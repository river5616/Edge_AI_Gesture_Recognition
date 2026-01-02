"""
手势定义配置文件
定义所有支持的手势及其属性
"""

# 手势ID映射（精简版 - 7个手势）
GESTURE_NAMES = {
    0: "fist",          # 拳头
    1: "palm",          # 手掌（五指展开）
    2: "thumbs_up",     # 大拇指向上
    3: "ok",            # OK手势
    4: "gun",           # 手枪手势
    5: "call_me",       # 打电话手势
    6: "nothing"        # 无手势/背景
}

# 手势中文名称
GESTURE_NAMES_CN = {
    0: "拳头",
    1: "手掌",
    2: "赞",
    3: "OK",
    4: "手枪",
    5: "打电话",
    6: "无"
}

# 手势描述（用于数据采集指导）
GESTURE_DESCRIPTIONS = {
    0: {
        "name": "fist",
        "cn_name": "拳头",
        "description": "五指完全握紧，拇指压在食指和中指上",
        "key_points": [
            "握紧到看不见手指",
            "拳头可以有不同朝向",
            "不要半握"
        ],
        "confusion_with": []
    },
    1: {
        "name": "palm",
        "cn_name": "手掌",
        "description": "五指完全展开并分开，手掌朝摄像头",
        "key_points": [
            "五指尽量分开",
            "拇指要明显看到（最重要）",
            "不要四指并拢"
        ],
        "confusion_with": []
    },
    2: {
        "name": "thumbs_up",
        "cn_name": "大拇指向上",
        "description": "拇指竖直向上，其他四指握拳",
        "key_points": [
            "拇指要完全伸直",
            "其他手指完全握紧",
            "不要食指伸出"
        ],
        "confusion_with": []
    },
    3: {
        "name": "ok",
        "cn_name": "OK手势",
        "description": "拇指+食指圈成圆圈，其他三指伸直或自然弯曲",
        "key_points": [
            "圆圈要明显",
            "正面看（圆圈朝摄像头）",
            "圆圈不要太小"
        ],
        "confusion_with": []
    },
    4: {
        "name": "gun",
        "cn_name": "手枪",
        "description": "拇指向上，食指向前，其他手指握拳，形成L形",
        "key_points": [
            "拇指和食指成90°直角",
            "食指要完全伸直",
            "中指要完全握紧",
            "不要让中指伸出",
            "不要让拇指贴近食指"
        ],
        "confusion_with": []
    },
    5: {
        "name": "call_me",
        "cn_name": "打电话",
        "description": "拇指和小指伸出，其他手指握拳，形成电话形状",
        "key_points": [
            "拇指和小指要完全伸直",
            "三个中间手指握紧",
            "可以侧面展示（更明显）",
            "不要让其他手指伸出"
        ],
        "confusion_with": []
    },
    6: {
        "name": "nothing",
        "cn_name": "无手势",
        "description": "空背景或手不在画面中",
        "key_points": [
            "完全空的背景",
            "或手完全离开画面"
        ],
        "confusion_with": []
    }
}

# 手势特征（用于可视化和分析）
GESTURE_FEATURES = {
    0: {"shape": "compact", "fingers_extended": 0, "distinctive": "closed_fist"},
    1: {"shape": "open", "fingers_extended": 5, "distinctive": "all_fingers_spread"},
    2: {"shape": "single_up", "fingers_extended": 1, "distinctive": "thumb_up"},
    3: {"shape": "circle", "fingers_extended": 4, "distinctive": "thumb_index_circle"},
    4: {"shape": "l_shape", "fingers_extended": 2, "distinctive": "thumb_up_index_forward"},
    5: {"shape": "two_ends", "fingers_extended": 2, "distinctive": "thumb_pinky_extended"},
    6: {"shape": "none", "fingers_extended": 0, "distinctive": "background"}
}

# 推荐的采集数量（每个手势）
RECOMMENDED_SAMPLES = {
    "minimum": 300,   # 最少
    "recommended": 500,  # 推荐
    "optimal": 1000   # 最佳
}

# 易混淆手势对（新方案应该没有）
CONFUSION_PAIRS = []  # 空列表，因为新方案手势互不相似

# 训练时的类别权重（所有手势同等重要）
CLASS_WEIGHTS = [1.0] * 7

def get_gesture_name(gesture_id):
    """获取手势英文名称"""
    return GESTURE_NAMES.get(gesture_id, "unknown")

def get_gesture_name_cn(gesture_id):
    """获取手势中文名称"""
    return GESTURE_NAMES_CN.get(gesture_id, "未知")

def get_gesture_description(gesture_id):
    """获取手势详细描述"""
    return GESTURE_DESCRIPTIONS.get(gesture_id, {})

def print_all_gestures():
    """打印所有手势信息"""
    print("=" * 80)
    print("手势识别系统 - 手势定义（精简版 - 7个手势）")
    print("=" * 80)
    print()

    for gid in range(7):
        desc = GESTURE_DESCRIPTIONS[gid]
        print(f"{gid}. {desc['cn_name']} ({desc['name']})")
        print(f"   描述: {desc['description']}")
        print(f"   要点:")
        for point in desc['key_points']:
            print(f"     - {point}")
        print()

    print("=" * 80)
    print("采集建议:")
    print(f"  最少: {RECOMMENDED_SAMPLES['minimum']} 张/手势")
    print(f"  推荐: {RECOMMENDED_SAMPLES['recommended']} 张/手势")
    print(f"  最佳: {RECOMMENDED_SAMPLES['optimal']} 张/手势")
    print("=" * 80)

if __name__ == "__main__":
    print_all_gestures()
