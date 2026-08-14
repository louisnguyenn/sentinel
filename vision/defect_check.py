import random
from pathlib import Path
import cv2
import numpy as np

SAMPLE_DIR = Path(__file__).parent.parent / "data" / "sample_parts"

def capture_current_part_image() -> str:
    """
    Stand-in for a real camera capture. Picks a random sample image so
    the pipeline can be exercised end-to-end without real hardware.
    """
    category = random.choice(["ok", "defective"])
    folder = SAMPLE_DIR / category
    image_path = random.choice(list(folder.glob("*.jpg")) + list(Path(folder).glob("*.jpeg")))
    
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

    # filter by area threshold first, avoids counting dust particles and other tiny contours as a real defect
    suspicious = [c for c in contours if cv2.contourArea(c) > contour_area_threshold]

    return len(suspicious) > suspicious_region_threshold

def evaluate_on_folder(folder: str, expected_defective: bool) -> None:
    correct = 0
    total = 0
    image_paths = list(Path(folder).glob("*.jpg")) + list(Path(folder).glob("*.jpeg"))

    for image_path in image_paths:
        result = detect_surface_defect(str(image_path))
        total += 1
        if result == expected_defective:
            correct += 1
    print(f"{folder}: {correct}/{total} correct")

def extract_features(image_path:str) -> list[float]:
    """
    Turns one image into a handful of numeric features for a classical ML classifier.
    """
    img = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        raise FileNotFoundError(f"Could not read image: {image_path}")

    blurred = cv2.GaussianBlur(img, (5, 5), 0)
    edges = cv2.Canny(blurred, 50, 150)
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    #   - number of contours found
    #   - the largest single contour's area
    #   - the mean pixel intensity of the whole image
    #   - the standard deviation of pixel intensity (texture roughness)
    if len(contours) > 0:
        contour_area = []
        for c in contours:
            contour_area.append(cv2.contourArea(c))
        max_contour_area = max(contour_area) # find largest contour area
    else:
        max_contour_area = 0 # no defects, scratches, etc.

    return [len(contours), max_contour_area, np.mean(img), np.std(img)]

if __name__ == "__main__":
    evaluate_on_folder(str(SAMPLE_DIR / "ok"), expected_defective=False)
    evaluate_on_folder(str(SAMPLE_DIR / "defective"), expected_defective=True)
    # sample = capture_current_part_image()
    # print(sample)
    # print(extract_features(sample))
