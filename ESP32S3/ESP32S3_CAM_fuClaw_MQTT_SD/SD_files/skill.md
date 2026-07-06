
==================================================
BUILT-IN SKILLS REGISTRY
==================================================

==================================================
SKILL: anti_theft_detection
==================================================

Goal:
Detect human presence and trigger alert workflow.

--------------------------------------------------
SKILL EXECUTION
--------------------------------------------------

MUST OUTPUT EXACT JSON ARRAY ONLY:

Step 1: Analyze image for human presence

{
  "type": "tool_call",
  "method": "/vision",
  "params": {
    "query": "Determine whether a person is visible in the image.",
    "frames": true,
    "task": "If a person is detected, continue workflow. If no person is detected, return NONE."
  }
}

Step 2: If person detected → trigger alert sequence

[
  {
    "type": "tool_call",
    "method": "/still",
    "params": {
      "frames": false,
      "task": "NONE"
    }
  },
  {
    "type": "tool_call",
    "method": "/digitalwrite",
    "params": {
      "pin": <blue_led_pin>,
      "pinmode": "digitalwrite",
      "value": 1
    }
  },
  {
    "type": "tool_call",
    "method": "/delay",
    "params": {
      "milliseconds": 500
    }
  },
  {
    "type": "tool_call",
    "method": "/digitalwrite",
    "params": {
      "pin": <blue_led_pin>,
      "pinmode": "digitalwrite",
      "value": 0
    }
  },
  {
    "type": "tool_call",
    "method": "/delay",
    "params": {
      "milliseconds": 500
    }
  }
]

--------------------------------------------------
FALLBACK
--------------------------------------------------

If uncertain → return natural conversational response.