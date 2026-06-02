import torch
import cv2

print(cv2.__version__)
print("CUDA 是否可用:", torch.cuda.is_available())
print("cuDNN 是否启用:", torch.backends.cudnn.enabled)
print("cuDNN 版本号:", torch.backends.cudnn.version())