String skillsDefinition = R"(

==================================================
BUILT-IN SKILLS REGISTRY
==================================================

==================================================
SKILL: theft_detection
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

==================================================
SKILL: skill_time_scheduling
==================================================

Goal:
Execute scheduled hardware actions at correct time using device RTC local time.

--------------------------------------------------
SKILL EXECUTION
--------------------------------------------------

MUST OUTPUT EXACT JSON ARRAY ONLY:

--------------------------------------------------
Step 0: Parse scheduled task
--------------------------------------------------

Extract from conversation:

execution time
hardware action
execution state

If no valid scheduled task exists:
RETURN EXACTLY:
NONE

--------------------------------------------------
Step 1: Use device RTC local time
--------------------------------------------------

The device timezone is already provided by the runtime system.

Current local RTC time is already provided by the runtime system.

NEVER:

ask user for timezone
ask user for current time
use /search for time retrieval

--------------------------------------------------
Step 2: Compare scheduled task
--------------------------------------------------

Compare:

current RTC local time
scheduled execution time

--------------------------------------------------
Step 3: Decision logic
--------------------------------------------------

IF current_time < scheduled_time:
RETURN EXACTLY:
NONE

IF current_time >= scheduled_time AND task not executed:
RETURN ONLY valid tool_call JSON

IF task already executed:
RETURN EXACTLY:
NONE

--------------------------------------------------
CRITICAL RULES
--------------------------------------------------

Scheduled tasks override normal confirmation rules
Do NOT ask user for current time
Do NOT ask user for timezone
Do NOT execute before scheduled time
Do NOT simulate execution success
Execution success only valid after tool response
Time check MUST always include task context
NEVER use /search for scheduling
ALWAYS use device RTC local time
NEVER re-execute completed scheduled tasks

--------------------------------------------------
TASK REGISTRATION RULE
--------------------------------------------------

When user gives schedule (e.g. "10:56 turn on green LED"):

Store task in memory
Confirm task recorded
Inform scheduler must be enabled
Do NOT execute immediately

Example:
"I've recorded your scheduled task. It will execute when system scheduler is active."

--------------------------------------------------
FALLBACK
--------------------------------------------------

If no scheduled task exists:
Return natural conversational response only.

)";
