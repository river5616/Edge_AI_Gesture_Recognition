#!/usr/bin/env python3
"""Programmatically analyze wrong samples to infer failure modes.

Computes per-image features that distinguish thumbs_up from fist:
- top_band_brightness: mean brightness of top 25% rows (where thumb usually lives)
- vertical_extent: height span of foreground pixels
- protrusion_score: ratio of foreground pixels in top band vs middle band
- foreground_area: total foreground pixel count
- contrast: pixel std
- ink_ratio: fraction of "dark" pixels (assuming hand is dark on light bg, or vice versa)
- mean_brightness: overall brightness (lighting check)

For ok vs palm:
- center_hole_score: how prominent is a closed loop in the center
- finger_separation: number of distinct vertical strips in finger area

Outputs per-sample features as CSV + a summary by failure mode bucket.
"""
import argparse
import csv
import os
import cv2
import numpy as np


def to_foreground_mask(img):
    """Auto threshold: assume hand region is whichever (bright or dark) has fewer pixels."""
    blur = cv2.GaussianBlur(img, (3, 3), 0)
    _, m1 = cv2.threshold(blur, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    fg = m1 if (m1 > 0).sum() < (m1 == 0).sum() else 255 - m1
    return fg


def features_for_image(img):
    h, w = img.shape
    mask = to_foreground_mask(img)

    fg_pix = (mask > 0).astype(np.uint8)
    fg_area = int(fg_pix.sum())

    top_band = mask[:h // 4, :]
    mid_band = mask[h // 4:3 * h // 4, :]

    top_brightness = float(img[:h // 4, :].mean())
    mid_brightness = float(img[h // 4:3 * h // 4, :].mean())

    top_fg = int((top_band > 0).sum())
    mid_fg = int((mid_band > 0).sum())

    if fg_area > 0:
        ys, xs = np.where(fg_pix > 0)
        v_extent = int(ys.max() - ys.min() + 1) if len(ys) else 0
        protrusion = top_fg / max(1, mid_fg)
    else:
        v_extent = 0
        protrusion = 0.0

    contrast = float(img.std())
    mean_b = float(img.mean())

    return {
        'fg_area': fg_area,
        'top_fg': top_fg,
        'mid_fg': mid_fg,
        'protrusion': protrusion,
        'v_extent': v_extent,
        'top_brightness': top_brightness,
        'mid_brightness': mid_brightness,
        'contrast': contrast,
        'mean_b': mean_b,
    }


def categorize_thumbs_up_to_fist(feat, ref):
    """Decide failure mode for thumbs_up samples being predicted as fist.

    ref = mean features of correctly-classified thumbs_up images (baseline)
    """
    notes = []

    # Compare protrusion with reference. Low protrusion = thumb invisible.
    if feat['protrusion'] < ref['protrusion'] * 0.5:
        notes.append('thumb_missing')
    elif feat['protrusion'] < ref['protrusion'] * 0.8:
        notes.append('thumb_weak')

    # Vertical extent shorter -> hand cropped or close fist
    if feat['v_extent'] < ref['v_extent'] * 0.85:
        notes.append('short_extent')

    # Brightness anomaly
    if feat['mean_b'] < ref['mean_b'] - 25 or feat['mean_b'] > ref['mean_b'] + 25:
        notes.append('lighting')

    # Contrast anomaly
    if feat['contrast'] < ref['contrast'] * 0.7:
        notes.append('low_contrast')

    if not notes:
        notes.append('unclear')
    return ','.join(notes)


def categorize_ok_to_palm(feat, ref):
    notes = []
    # OK has a small ring/hole; palm is fully open. The ring contributes less foreground area
    # because the loop is hollow. So OK should have LOWER fg_area than palm.
    # If a misclassified OK has fg_area close to palm's, the ring is closed/filled.
    # We compare to ref (correct OK average).
    if feat['fg_area'] > ref['fg_area'] * 1.15:
        notes.append('ring_filled')
    if feat['mean_b'] < ref['mean_b'] - 25 or feat['mean_b'] > ref['mean_b'] + 25:
        notes.append('lighting')
    if feat['contrast'] < ref['contrast'] * 0.7:
        notes.append('low_contrast')
    if feat['v_extent'] < ref['v_extent'] * 0.85:
        notes.append('short_extent')
    if not notes:
        notes.append('unclear')
    return ','.join(notes)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--csv', required=True, help='wrong samples csv from eval_onnx_dataset')
    ap.add_argument('--dataset', required=True, help='hard test dataset root (parent of class dirs)')
    ap.add_argument('--out', default='wrong_features.csv')
    ap.add_argument('--ref-thumbs', default=None, help='dir of correctly-classified thumbs_up (defaults to dataset/2_thumbs_up)')
    ap.add_argument('--ref-ok', default=None, help='dir of correctly-classified ok (defaults to dataset/3_ok)')
    args = ap.parse_args()

    # Load wrong samples
    rows = []
    with open(args.csv, 'r', encoding='utf-8') as f:
        for r in csv.DictReader(f):
            r['confidence'] = float(r['confidence'])
            r['margin'] = float(r['margin'])
            rows.append(r)

    # Compute per-class reference (correctly classified set assumed = full class dir minus wrong)
    wrong_paths = {r['path'] for r in rows}

    def load_class_features(cls_dir):
        feats = []
        for fn in sorted(os.listdir(cls_dir)):
            if not fn.lower().endswith(('.png', '.jpg', '.jpeg')):
                continue
            full = os.path.join(cls_dir, fn)
            rel_norm = full.replace('/', '\\')
            # Skip if this sample is wrong (we want reference = correct samples only)
            skip = False
            for wp in wrong_paths:
                if os.path.basename(wp) == fn and os.path.basename(os.path.dirname(wp)) == os.path.basename(cls_dir):
                    skip = True
                    break
            if skip:
                continue
            img = cv2.imread(full, cv2.IMREAD_GRAYSCALE)
            if img is None:
                continue
            feats.append(features_for_image(img))
        if not feats:
            return None
        keys = feats[0].keys()
        ref = {k: float(np.mean([f[k] for f in feats])) for k in keys}
        return ref

    thumbs_dir = args.ref_thumbs or os.path.join(args.dataset, '2_thumbs_up')
    ok_dir = args.ref_ok or os.path.join(args.dataset, '3_ok')

    ref_thumbs = load_class_features(thumbs_dir)
    ref_ok = load_class_features(ok_dir)

    print('Reference (correct thumbs_up):', {k: f'{v:.2f}' for k, v in ref_thumbs.items()})
    print('Reference (correct ok):       ', {k: f'{v:.2f}' for k, v in ref_ok.items()})
    print()

    out_rows = []
    bucket_counts = {}
    for r in rows:
        rel_path = r['path']
        full = os.path.join(args.dataset, *rel_path.split('\\')[1:]) if rel_path.startswith('dataset_hard_test') else rel_path
        if not os.path.exists(full):
            full = rel_path
        img = cv2.imread(full, cv2.IMREAD_GRAYSCALE)
        if img is None:
            continue
        feat = features_for_image(img)

        bucket = ''
        if r['true_name'] == 'thumbs_up' and r['pred_name'] == 'fist':
            bucket = categorize_thumbs_up_to_fist(feat, ref_thumbs)
        elif r['true_name'] == 'ok' and r['pred_name'] == 'palm':
            bucket = categorize_ok_to_palm(feat, ref_ok)

        if bucket:
            for tag in bucket.split(','):
                key = f"{r['true_name']}_to_{r['pred_name']}:{tag}"
                bucket_counts[key] = bucket_counts.get(key, 0) + 1

        out_rows.append({
            'path': rel_path,
            'true': r['true_name'],
            'pred': r['pred_name'],
            'conf': r['confidence'],
            'margin': r['margin'],
            'bucket': bucket,
            **{f'feat_{k}': f"{v:.3f}" for k, v in feat.items()},
        })

    with open(args.out, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=list(out_rows[0].keys()))
        w.writeheader()
        w.writerows(out_rows)

    print('=== Bucket counts (multi-tag, samples may appear in multiple) ===')
    for k in sorted(bucket_counts.keys()):
        print(f'  {k}: {bucket_counts[k]}')

    print()
    print(f'Saved {args.out} with {len(out_rows)} rows')


if __name__ == '__main__':
    main()
