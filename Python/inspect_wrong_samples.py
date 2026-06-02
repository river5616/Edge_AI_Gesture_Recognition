#!/usr/bin/env python3
"""Visualize wrong samples from a hard_test evaluation CSV as grids per error type."""
import argparse
import csv
import os
import math
import cv2
import numpy as np


def load_wrong_csv(csv_path):
    rows = []
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for r in reader:
            r['confidence'] = float(r['confidence'])
            r['margin'] = float(r['margin'])
            rows.append(r)
    return rows


def make_grid(items, base_dir, cell=128, cols=5, font_scale=0.4):
    if not items:
        return None
    rows = math.ceil(len(items) / cols)
    grid_w = cols * cell
    grid_h = rows * (cell + 28)
    canvas = np.full((grid_h, grid_w, 3), 30, dtype=np.uint8)

    for idx, it in enumerate(items):
        r = idx // cols
        c = idx % cols
        x0 = c * cell
        y0 = r * (cell + 28)

        path = it['path']
        img_path = os.path.join(base_dir, path) if not os.path.isabs(path) else path
        img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        if img is None:
            continue
        img_resized = cv2.resize(img, (cell, cell), interpolation=cv2.INTER_NEAREST)
        img_bgr = cv2.cvtColor(img_resized, cv2.COLOR_GRAY2BGR)
        canvas[y0:y0 + cell, x0:x0 + cell] = img_bgr

        fname = os.path.basename(path).replace('.png', '')
        label1 = f"{fname}"
        label2 = f"c={it['confidence']:.2f} m={it['margin']:.2f}"
        cv2.putText(canvas, label1, (x0 + 2, y0 + cell + 12),
                    cv2.FONT_HERSHEY_SIMPLEX, font_scale, (200, 200, 200), 1)
        cv2.putText(canvas, label2, (x0 + 2, y0 + cell + 25),
                    cv2.FONT_HERSHEY_SIMPLEX, font_scale, (180, 220, 255), 1)
    return canvas


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--csv', required=True)
    ap.add_argument('--base', default='.', help='base dir for relative paths in csv')
    ap.add_argument('--out-prefix', default='wrong_grid')
    ap.add_argument('--true', default=None, help='filter true class name')
    ap.add_argument('--pred', default=None, help='filter pred class name')
    ap.add_argument('--sort-by-conf', action='store_true')
    args = ap.parse_args()

    rows = load_wrong_csv(args.csv)
    by_pair = {}
    for r in rows:
        if args.true and r['true_name'] != args.true:
            continue
        if args.pred and r['pred_name'] != args.pred:
            continue
        key = f"{r['true_name']}_to_{r['pred_name']}"
        by_pair.setdefault(key, []).append(r)

    if args.sort_by_conf:
        for k in by_pair:
            by_pair[k].sort(key=lambda r: -r['confidence'])

    for key, items in sorted(by_pair.items(), key=lambda kv: -len(kv[1])):
        grid = make_grid(items, args.base)
        if grid is None:
            continue
        out_path = f"{args.out_prefix}_{key}_n{len(items)}.png"
        cv2.imwrite(out_path, grid)
        print(f"Saved {out_path} ({len(items)} samples)")


if __name__ == '__main__':
    main()
