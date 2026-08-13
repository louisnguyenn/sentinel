import random
from pathlib import Path
import cv2

SAMPLE_DIR = Path(__file__).parent.parent / "data" / "sample_parts"

def capture_current_part_image() -> str:
    """
    Stand-in for a real camera capture. Picks a random sample image so
    the pipeline can be exercised end-to-end without real hardware.
    """
    category = random.choice(["ok", "defective"])
    folder = SAMPLE_DIR / category
    image_path = random.choice(list(folder.glob("*.jpg")))
    
    return str(image_path)


def detect_surface_defect(image_path:str, contour_area_threshold: float = 40.0, suspicious_region_threshold: int = 3) -> bool:
    """
    Classical CV defect check: grayscale -> blur -> Canny edges -> contour count.
    Returns True if the part looks defective, False if it looks OK.
    """
    img = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE) # grayscale
    if img is None:
        raise FileNotFoundError(f"Could not read image: {image_path}")

    blurred = cv2.GaussianBlur(img, (5, 5), 0) # blur
    edges = cv2.Canny(blurred, 50, 150) # canny edges
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) # contour count

    # filter by area thresholf first, avoids counting dust particles and other tiny contours as a real defect
    suspicious = [c for c in contours if cv2.contourArea(c) > contour_area_threshold]

    return len(suspicious) > suspicious_region_threshold
