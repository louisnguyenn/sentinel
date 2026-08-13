import random
from pathlib import Path

SAMPLE_DIR = Path(__file__).parent.parent / "data" / "sample_parts"

def capture_current_part_image() -> str:
    category = random.choice(["ok", "defective"])
    folder = SAMPLE_DIR / category
    image_path = random.choice(list(folder.glob("*.jpg")))
    
    return str(image_path)
    