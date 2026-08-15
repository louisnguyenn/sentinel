# Phase 4 — Lean Python Vision Module

## Vision's job, at a high level
1. Watch `REG_TRIGGER_CAPTURE`
2. When triggered: capture/load image → classify → post result
   - writes `REG_INSPECTION_RESULT`, `REG_RESULT_SEQ`
3. Increment `REG_VISION_HEARTBEAT` every loop iteration
   - unconditional — keeps the watchdog satisfied regardless of whether a
     trigger happened that iteration

## Edge detection on the Python side
`TRIGGER_CAPTURE` is a level signal (stays 1 for the whole `PART_DETECTED` +
`AWAIT_RESULT`-before-result window), not a one-tick pulse — made deliberately
level-based so a missed poll can't cause vision to miss a request entirely.

That means vision has to do its own rising-edge detection, same shape as the
controller's `IDLE` photoeye check:
```python
if current_trigger == 1 and previous_trigger == 0:
    # capture and classify — only on the moment it turns on
```
Reacting to "current != previous" isn't enough — that would also fire on the
falling edge (1 → 0), which should do nothing. Needs the specific direction.

## Part 1: Classical CV logic

Four operations, in order:
- **Grayscale** — dropping colour; colour is irrelevant for surface defects, and it's
  simpler/faster to work with one number per pixel instead of three
- **Gaussian blur** — smooths out random noise (sensor grain, lighting flicker)
  *before* edge detection, so noise itself doesn't get mistaken for a real edge
- **Canny edge detection** — finds sharp brightness transitions in the image;
  a sharp local change in surface reflectance is what a scratch, crack, or
  chip actually looks like on a metal part
- **Contours** — groups connected edge pixels into distinct shapes
  - defect → one or more real contours
  - clean surface → very few or no contours (in theory)

Filtered by contour *area* before counting, to avoid counting dust specks /
tiny noise contours as if they were real defects.

### Result on the real dataset
Ran `evaluate_on_folder()` against the Kaggle casting dataset:
- `ok` folder: ~30% correct
- `defective` folder: ~65% correct

Worse than random guessing on the `ok` class — the area-threshold approach
doesn't cleanly separate the two classes on this dataset. Printed raw feature
values (contour count, max contour area) side by side for both classes and
found the ranges overlap heavily — area in particular does **not** separate
`ok` vs `defective` well here (the single largest area in one sample batch was
actually from an `ok` image). Contour *count* showed some real signal (defective
trending higher) but was noisy.

Conclusion: classical single-threshold detection isn't reliable enough for
this dataset on its own — moved to the ML layer rather than continuing to
hand-tune thresholds against a feature that doesn't separate the classes well.

`detect_surface_defect()` stays in the codebase as a documented baseline /
reference point, not as a live fallback (see decision below).

## Part 2: Machine learning layer

### Feature extraction
Instead of raw pixels, extract a small number of engineered numbers per image
(`extract_features()`):
1. total contour count
2. largest single contour's area (0 if no contours — a genuinely clean image
   has zero-size "largest defect," not an undefined one)
3. mean pixel intensity of the whole grayscale image
4. standard deviation of pixel intensity (rough measure of surface texture —
   a scratched/pitted surface has more brightness variation than a smooth one)

### Training
`train_classifier.py` — RandomForestClassifier from scikit-learn, trained on
features extracted from the full labeled dataset.
- `stratify=y` on the train/test split — dataset isn't perfectly balanced
  (3,399 ok vs 4,613 defective), so this keeps the same class ratio in both
  train and test sets instead of leaving it to chance
- `random_state=42` fixed on both the split and the model — reproducible runs
- Saved with `joblib.dump()` → `detect_classifier.joblib` (ignored in .gitignore, must train first when cloning repo)

### Result
~97% accuracy on the held-out test set, recall of 0.98 on the `defective`
class specifically (catches ~98% of real defects) — a large improvement over
the classical approach's ~50% blended accuracy.

**Caveat worth remembering:** 97% is high enough to be worth a moment of
skepticism — possible data leakage (near-duplicate images across train/test)
given this is a public Kaggle dataset, not something independently verified.
Worth noting as a caveat rather than presenting as an unqualified real-world
number.

### Wiring into inspector.py
`classify_with_model()` loads the trained model **once**, lazily, via a
module-level `_model` global — loading from disk on every single
classification call would be far too slow inside a loop polling every 20ms.

**Decision: no classical-detector fallback.** If the trained model file is
missing, vision fails loudly (`FileNotFoundError`) rather than silently
degrading to the much less reliable classical detector. Consistent with the
project's overall philosophy: don't trust ambiguous/degraded data, fail
visibly rather than quietly do something worse.

## Bugs hit this phase (worth remembering)
- **Path resolution bug** — `evaluate_on_folder()` used a path relative to
  wherever the script was *launched from*, not relative to the script file's
  own location. Silently found zero images instead of erroring, because
  globbing a nonexistent folder just returns empty. Fixed by consistently
  using `Path(__file__).parent...`-based paths everywhere, matching what
  `capture_current_part_image()` already did correctly.
- **`max(contours)` type mismatch** — `contours` is a list of point-arrays,
  not areas; `max()` on it directly doesn't compute "largest area." Needed
  `cv2.contourArea(c)` applied to each contour first.
- **Missing `else` on empty-contours case** — same shape of bug as the
  controller's `AWAIT_RESULT` bug from Phase 2/3: an `if` with no `else` left
  a variable unassigned when `contours` was empty, causing a `NameError`.
- **scikit-learn 1D vs 2D input** — `model.predict()` always expects a 2D
  array (rows = samples, columns = features), even for a single sample.
  `extract_features()` returns one flat list per image (1D); had to wrap it
  as `[features]` at inference time to represent "a batch of one sample."
- **pymodbus API version mismatch** — `ModbusTcpClient(host, port)`
  positional args didn't match the installed pymodbus version's constructor;
  fixed using explicit keyword args (`host=`, `port=`).
- **Sampling-interval illusion** — printed controller state every 100 ticks;
  state appeared to "jump" straight to `DIVERT_REJECT` with no visible
  `PART_DETECTED`/`AWAIT_RESULT` in between. Not an actual bug — the real
  transitions were happening between the fixed print intervals, invisible at
  that sample rate. Switched to logging on *state change* rather than on a
  fixed tick interval — a generally better debugging pattern than periodic
  sampling.
- **Unthrottled scan cycle** — controller loop had no real-time pacing at
  all, spinning far faster than `dt` implied. Fixed by measuring each scan
  cycle's actual elapsed time and sleeping the remainder, so simulated time
  roughly tracks real time — same pattern originally sketched back in
  Phase 2's scan-cycle design, only actually implemented here once two
  independently-timed processes needed to stay in sync.
