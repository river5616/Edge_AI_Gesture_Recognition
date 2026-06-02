import numpy as np, cv2
from pathlib import Path
import onnxruntime as ort

G = {0:'fist',1:'palm',2:'thumbs_up',3:'ok',4:'nothing'}

def softmax(x):
    x = x.astype(np.float32); x = x - x.max(); e = np.exp(x); return e / e.sum()

import sys
model = sys.argv[1] if len(sys.argv) > 1 else 'gesture_v1_5.onnx'
s = ort.InferenceSession(model, providers=['CPUExecutionProvider'])
inn = s.get_inputs()[0].name
out = s.get_outputs()[0].name

ds = Path('dataset_real')
if not ds.exists():
    ds = Path('dataset_hard_test')
print('模型:', model)
print('数据集:', ds)
print()

for cid, cn in G.items():
    d = ds / f'{cid}_{cn}'
    imgs = (sorted(d.glob('*.png')) + sorted(d.glob('*.jpg'))) if d.exists() else []
    if not imgs:
        print(f'{cn}: 无样本'); continue
    # 取3张看稳定性
    for img_path in imgs[:3]:
        img = cv2.imread(str(img_path), cv2.IMREAD_GRAYSCALE)
        if img is None: continue
        if img.shape != (64, 64):
            img = cv2.resize(img, (64, 64), interpolation=cv2.INTER_AREA)
        x = ((img.astype(np.float32) / 255 - 0.5) / 0.5).reshape(1, 1, 64, 64)
        p = softmax(s.run([out], {inn: x})[0][0])
        pred = int(p.argmax())
        allp = ' '.join(f'{v:.3f}' for v in p)
        mark = '' if pred == cid else '  <-- 错'
        print(f'真值={cn:10s} 预测={G[pred]:10s} 置信={p[pred]:.4f}  [{allp}]{mark}')
    print()
