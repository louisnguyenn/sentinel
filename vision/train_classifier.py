from pathlib import Path
import joblib
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report

from defect_check import extract_features

SAMPLE_DIR = Path(__file__).parent.parent / "data" / "sample_parts"
MODEL_PATH = Path(__file__).parent / "detect_classifier.joblib"

def load_dataset(based_dir: Path):
    x, y, = [], []
    for label, folder_name in [(0, "ok"), (1, "defective")]:
        folder = based_dir / folder_name
        image_paths = list(folder.glob("*.jpg")) + list(folder.glob("*.jpeg"))
        for image_path in image_paths:
            x.append(extract_features(str(image_path)))
            y.append(label)
    return x, y

if __name__ == "__main__":
    print("Loading dataset and extracting features...")
    x, y = load_dataset(SAMPLE_DIR)
    print(f"Loading {len(x)} images and ({sum(y)} defective, {len(y) - sum(y)} ok)")

    # stratify=y to split train/test evenly, fix random_state for same split and trained model
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.2, random_state=42, stratify=y)

    clf = RandomForestClassifier(n_estimators=100, random_state=42)
    clf.fit(x_train, y_train)

    print("\n--- Test Set Performance ---")
    print(classification_report(y_test, clf.predict(x_test), target_names=["ok", "defective"]))

    joblib.dump(clf, MODEL_PATH)
    print(f"Saved model to {MODEL_PATH}")
    