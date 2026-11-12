import json
import urllib.request
from pathlib import Path

# AREDL's public API exposes level data including placement.
# The exact API schema can evolve; keep parsing isolated in this script.
URL = "https://api.aredl.net/api/aredl/levels"

out = Path("data/levels.json")

req = urllib.request.Request(
    URL,
    headers={"User-Agent": "Extended-Demon-Stars/1.0"}
)

try:
    with urllib.request.urlopen(req, timeout=30) as response:
        payload = json.load(response)
except Exception as exc:
    print(f"AREDL download failed: {exc}")
    raise SystemExit(0)

levels = payload.get("levels", payload if isinstance(payload, list) else [])
result = {}

total = len(levels)

for item in levels:
    try:
        level_id = int(item.get("id", item.get("level_id")))
        placement = int(item.get("position", item.get("placement", 0)))
    except (TypeError, ValueError):
        continue

    result[str(level_id)] = {
        "placement": placement,
        "total": total
    }

out.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
print(f"Wrote {len(result)} AREDL levels")
