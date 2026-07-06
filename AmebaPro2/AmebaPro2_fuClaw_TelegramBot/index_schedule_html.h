#ifndef INDEX_SCHEDULE_HTML_H
#define INDEX_SCHEDULE_HTML_H

const char INDEX_SCHEDULE_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>fuClaw Task Scheduler</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@400;500;600;700&family=Noto+Sans+TC:wght@300;400;500&family=JetBrains+Mono:wght@400;500&display=swap');

  :root {
    --bg:       #f0f4ff;
    --surface:  #ffffff;
    --surface2: #f7f9ff;
    --border:   #dde3f5;
    --border2:  #c4cef5;
    --accent:   #4f6ef7;
    --accent2:  #10b981;
    --accent3:  #06b6d4;
    --warn:     #f59e0b;
    --danger:   #ef4444;
    --success:  #10b981;
    --text:     #1a1d2e;
    --text-dim: #8892b0;
    --text2:    #4a5270;
    --radius:   12px;
    --mono: 'JetBrains Mono', 'Courier New', monospace;
    --sans: 'Noto Sans TC', 'Space Grotesk', sans-serif;
  }

  * { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    background: var(--bg);
    color: var(--text);
    font-family: var(--sans);
    min-height: 100vh;
    overflow-x: hidden;
  }

  body::before {
    content: '';
    position: fixed;
    inset: 0;
    background:
      radial-gradient(circle at 20% 10%, rgba(79,110,247,.08) 0%, transparent 50%),
      radial-gradient(circle at 80% 80%, rgba(124,58,237,.07) 0%, transparent 50%),
      radial-gradient(circle at 60% 30%, rgba(6,182,212,.06) 0%, transparent 40%);
    pointer-events: none;
    z-index: 0;
  }



  .container {
    max-width: 1100px;
    margin: 0 auto;
    padding: 24px 16px 60px;
    position: relative;
    z-index: 1;
  }

  /* Header */
  header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    flex-wrap: wrap;
    gap: 12px;
    margin-bottom: 28px;
    padding-bottom: 18px;
    border-bottom: 1px solid var(--border);
  }

  .logo {
    display: flex;
    align-items: center;
    gap: 12px;
  }

  .chip-icon {
    width: 40px;
    height: 40px;
    flex-shrink: 0;
  }

  .logo h1 {
    font-family: var(--mono);
    font-size: 1.1rem;
    color: var(--accent);
    letter-spacing: 0.08em;
    line-height: 1.2;
  }

  .logo h1 span {
    display: block;
    font-size: 0.65rem;
    color: var(--text-dim);
    letter-spacing: 0.12em;
    margin-top: 2px;
  }

  .status-bar {
    display: flex;
    align-items: center;
    gap: 16px;
    font-family: var(--mono);
    font-size: 0.72rem;
    color: var(--text-dim);
  }

  .status-dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    background: var(--text-dim);
    display: inline-block;
    margin-right: 6px;
    position: relative;
  }

  .status-dot.online {
    background: var(--success);
    box-shadow: 0 0 8px var(--success);
    animation: pulse 2s infinite;
  }

  @keyframes pulse {
    0%,100% { opacity: 1; }
    50% { opacity: 0.4; }
  }

  /* Toolbar */
  .toolbar {
    display: flex;
    flex-wrap: wrap;
    gap: 10px;
    margin-bottom: 20px;
    align-items: center;
  }

  .btn {
    font-family: var(--mono);
    font-size: 0.78rem;
    letter-spacing: 0.06em;
    padding: 8px 18px;
    border: 1px solid;
    border-radius: var(--radius);
    cursor: pointer;
    background: transparent;
    transition: all 0.18s;
    display: inline-flex;
    align-items: center;
    gap: 7px;
    white-space: nowrap;
  }

  .btn-primary {
    border-color: var(--accent);
    color: var(--accent);
  }
  .btn-primary:hover {
    background: var(--accent);
    color: #fff;
    box-shadow: 0 4px 16px rgba(79,110,247,0.35);
  }

  .btn-success {
    border-color: var(--accent2);
    color: var(--accent2);
  }
  .btn-success:hover {
    background: var(--accent2);
    color: #fff;
    box-shadow: 0 4px 16px rgba(16,185,129,0.35);
  }

  .btn-danger {
    border-color: var(--danger);
    color: var(--danger);
  }
  .btn-danger:hover {
    background: var(--danger);
    color: #fff;
    box-shadow: 0 0 16px rgba(255,51,102,0.35);
  }

  .btn-warn {
    border-color: var(--warn);
    color: var(--warn);
  }
  .btn-warn:hover {
    background: var(--warn);
    color: #fff;
    box-shadow: 0 0 16px rgba(255,107,53,0.35);
  }

  .btn:disabled {
    opacity: 0.3;
    cursor: not-allowed;
    pointer-events: none;
  }

  .toolbar-right {
    margin-left: auto;
    display: flex;
    gap: 10px;
  }

  /* Stats bar */
  .stats {
    display: flex;
    gap: 20px;
    margin-bottom: 18px;
    flex-wrap: wrap;
  }

  .stat-item {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 8px 16px;
    font-family: var(--mono);
    font-size: 0.72rem;
    color: var(--text-dim);
    display: flex;
    align-items: center;
    gap: 8px;
  }

  .stat-item .val {
    font-size: 1.1rem;
    color: var(--accent);
    font-weight: bold;
  }

  /* Table */
  .table-wrap {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    overflow: hidden;
  }

  table {
    width: 100%;
    border-collapse: collapse;
    font-size: 0.82rem;
  }

  thead th {
    background: var(--surface2);
    color: var(--text-dim);
    font-family: var(--mono);
    font-size: 0.68rem;
    letter-spacing: 0.1em;
    text-transform: uppercase;
    padding: 11px 14px;
    text-align: left;
    border-bottom: 1px solid var(--border);
    white-space: nowrap;
  }

  tbody tr {
    border-bottom: 1px solid var(--border);
    transition: background 0.14s;
  }

  tbody tr:last-child { border-bottom: none; }
  tbody tr:hover { background: rgba(79,110,247,0.04); }

  tbody td {
    padding: 11px 14px;
    vertical-align: middle;
  }

  .task-cell {
    font-weight: 500;
    color: var(--text);
    max-width: 220px;
    word-break: break-all;
  }

  .offline-tag {
    display: inline-block;
    font-family: var(--mono);
    font-size: 0.62rem;
    font-weight: 600;
    letter-spacing: 0.04em;
    color: var(--accent2);
    background: rgba(16,185,129,0.1);
    border: 1px solid rgba(16,185,129,0.3);
    border-radius: 5px;
    padding: 1px 5px;
    margin-right: 5px;
    vertical-align: middle;
  }

  .time-cell {
    font-family: var(--mono);
    font-size: 0.75rem;
    color: var(--text);
    white-space: nowrap;
  }

  .time-cell .date-part { color: var(--accent); }
  .time-cell .time-part { color: var(--accent3); }
  .time-cell .wildcard { color: var(--warn); }

  .badge {
    font-family: var(--mono);
    font-size: 0.65rem;
    padding: 3px 9px;
    border-radius: 8px;
    letter-spacing: 0.08em;
    display: inline-block;
    white-space: nowrap;
  }

  .badge-done {
    background: rgba(16,185,129,0.1);
    color: var(--success);
    border: 1px solid rgba(16,185,129,0.3);
  }

  .badge-pending {
    background: rgba(245,158,11,0.1);
    color: var(--warn);
    border: 1px solid rgba(245,158,11,0.3);
  }

  .badge-locked {
    background: rgba(136,146,176,0.12);
    color: var(--text-dim);
    border: 1px solid var(--border);
  }

  .actions { white-space: nowrap; }

  .icon-btn {
    background: transparent;
    border: 1px solid transparent;
    cursor: pointer;
    color: var(--text-dim);
    padding: 5px 8px;
    border-radius: 8px;
    font-size: 0.85rem;
    transition: all 0.16s;
  }
  .icon-btn:hover.edit { color: var(--accent); border-color: var(--accent); }
  .icon-btn:hover.del  { color: var(--danger); border-color: var(--danger); }

  /* Empty state */
  .empty-state {
    text-align: center;
    padding: 60px 20px;
    color: var(--text-dim);
    font-family: var(--mono);
    font-size: 0.8rem;
  }
  .empty-state .big { font-size: 2.5rem; margin-bottom: 12px; opacity: 0.3; }

  /* Modal */
  .modal-overlay {
    display: none;
    position: fixed;
    inset: 0;
    background: rgba(10,15,40,0.55);
    z-index: 100;
    align-items: center;
    justify-content: center;
    backdrop-filter: blur(3px);
  }
  .modal-overlay.open { display: flex; }

  .modal {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    width: 90%;
    max-width: 560px;
    box-shadow: 0 8px 40px rgba(79,110,247,0.15), 0 2px 8px rgba(0,0,0,0.08);
    animation: fadeIn 0.18s ease;
    max-height: 90vh;
    overflow-y: auto;
  }

  @keyframes fadeIn {
    from { opacity: 0; transform: translateY(-12px); }
    to   { opacity: 1; transform: none; }
  }

  .modal-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 16px 20px;
    border-bottom: 1px solid var(--border);
    background: var(--surface2);
    border-radius: var(--radius) var(--radius) 0 0;
  }

  .modal-header h2 {
    font-family: var(--mono);
    font-size: 0.9rem;
    color: var(--accent);
    letter-spacing: 0.08em;
  }

  .modal-close {
    background: none;
    border: none;
    color: var(--text-dim);
    cursor: pointer;
    font-size: 1.2rem;
    padding: 2px 8px;
    border-radius: 2px;
    transition: color 0.15s;
  }
  .modal-close:hover { color: var(--danger); }

  .modal-body { padding: 20px; }
  .modal-footer {
    padding: 14px 20px;
    border-top: 1px solid var(--border);
    display: flex;
    justify-content: flex-end;
    gap: 10px;
  }

  /* Form */
  .form-row {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 14px;
    margin-bottom: 14px;
  }
  .form-row.single { grid-template-columns: 1fr; }
  .form-row.three  { grid-template-columns: 1fr 1fr 1fr; }

  .form-group label {
    display: block;
    font-family: var(--mono);
    font-size: 0.68rem;
    color: var(--text-dim);
    letter-spacing: 0.1em;
    margin-bottom: 6px;
    text-transform: uppercase;
  }

  .form-group input, .form-group select {
    width: 100%;
    background: var(--bg);
    border: 1px solid var(--border);
    color: var(--text);
    font-family: var(--mono);
    font-size: 0.82rem;
    padding: 8px 12px;
    border-radius: 8px;
    outline: none;
    transition: border-color 0.16s;
  }
  .form-group input:focus, .form-group select:focus {
    border-color: var(--accent);
    box-shadow: 0 0 0 3px rgba(79,110,247,0.12);
  }
  .form-group input.invalid { border-color: var(--danger); }

  .form-group small {
    display: block;
    font-size: 0.65rem;
    color: var(--text-dim);
    margin-top: 4px;
  }

  .section-label {
    font-family: var(--mono);
    font-size: 0.68rem;
    color: var(--text-dim);
    letter-spacing: 0.12em;
    text-transform: uppercase;
    margin: 18px 0 10px;
    padding-bottom: 6px;
    border-bottom: 1px solid var(--border);
    display: flex;
    align-items: center;
    gap: 8px;
  }

  .toggle-row {
    display: flex;
    align-items: center;
    gap: 12px;
    margin-top: 14px;
  }

  .toggle-wrap {
    display: flex;
    align-items: center;
    gap: 10px;
    font-family: var(--mono);
    font-size: 0.78rem;
    color: var(--text);
  }

  .toggle {
    position: relative;
    width: 40px;
    height: 22px;
    flex-shrink: 0;
  }
  .toggle input { opacity: 0; width: 0; height: 0; }
  .slider {
    position: absolute;
    inset: 0;
    background: var(--surface2);
    border: 1px solid var(--border);
    border-radius: 22px;
    cursor: pointer;
    transition: all 0.2s;
  }
  .slider::before {
    content: '';
    position: absolute;
    height: 14px; width: 14px;
    left: 3px; top: 3px;
    background: #cbd5e1;
    border-radius: 50%;
    transition: all 0.2s;
  }
  .toggle input:checked + .slider { background: rgba(16,185,129,0.15); border-color: var(--accent2); }
  .toggle input:checked + .slider::before { transform: translateX(18px); background: var(--success); }
  .toggle input:disabled + .slider { opacity: 0.35; cursor: not-allowed; }

  .hint-box {
    background: rgba(245,158,11,0.07);
    border: 1px solid rgba(245,158,11,0.25);
    border-radius: 8px;
    padding: 10px 14px;
    font-size: 0.72rem;
    color: var(--warn);
    font-family: var(--mono);
    margin-top: 14px;
    display: none;
  }
  .hint-box.show { display: block; }

  /* Confirm modal */
  .confirm-icon { font-size: 2.8rem; text-align: center; margin-bottom: 10px; }
  .confirm-msg { font-size: 0.88rem; line-height: 1.7; color: var(--text); margin-bottom: 6px; }
  .confirm-detail {
    font-family: var(--mono);
    font-size: 0.7rem;
    color: var(--text-dim);
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 10px 14px;
    margin-top: 12px;
    max-height: 140px;
    overflow-y: auto;
    line-height: 1.6;
  }

  /* Toast */
  #toast {
    position: fixed;
    bottom: 82px;
    right: 24px;
    z-index: 200;
    display: flex;
    flex-direction: column;
    gap: 8px;
    pointer-events: none;
  }

  .toast-item {
    font-family: var(--mono);
    font-size: 0.75rem;
    letter-spacing: 0.05em;
    padding: 10px 18px;
    border-radius: 8px;
    border: 1px solid;
    pointer-events: none;
    animation: slideIn 0.22s ease, fadeOut 0.3s ease 2.7s forwards;
    max-width: 320px;
  }

  .toast-item.success {
    background: rgba(16,185,129,0.1);
    border-color: var(--success);
    color: #059669;
  }
  .toast-item.error {
    background: rgba(239,68,68,0.1);
    border-color: var(--danger);
    color: var(--danger);
  }
  .toast-item.info {
    background: rgba(79,110,247,0.1);
    border-color: var(--accent);
    color: var(--accent);
  }

  @keyframes slideIn {
    from { opacity: 0; transform: translateX(30px); }
    to   { opacity: 1; transform: none; }
  }
  @keyframes fadeOut {
    to { opacity: 0; transform: translateX(30px); }
  }

  /* Loading */
  .loading-overlay {
    display: none;
    position: fixed;
    inset: 0;
    background: rgba(240,244,255,0.75);
    z-index: 300;
    align-items: center;
    justify-content: center;
    flex-direction: column;
    gap: 16px;
    backdrop-filter: blur(2px);
  }
  .loading-overlay.show { display: flex; }

  .spinner {
    width: 40px; height: 40px;
    border: 2px solid var(--border);
    border-top-color: var(--accent);
    border-radius: 50%;
    animation: spin 0.8s linear infinite;
  }
  @keyframes spin { to { transform: rotate(360deg); } }

  .loading-text {
    font-family: var(--mono);
    font-size: 0.75rem;
    color: var(--accent);
    letter-spacing: 0.1em;
    animation: blink 1s step-end infinite;
  }
  @keyframes blink { 50% { opacity: 0.3; } }

  /* Responsive */
  @media (max-width: 700px) {
    .hide-sm { display: none !important; }
    .form-row { grid-template-columns: 1fr; }
    .form-row.three { grid-template-columns: 1fr 1fr; }
    thead th:nth-child(4) { display: none; }
    tbody td:nth-child(4) { display: none; }
  }
</style>
</head>
<body>

<div class="container">
  <!-- Header -->
  <header>
    <div class="logo">
      <svg class="chip-icon" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
        <rect x="10" y="10" width="20" height="20" rx="2" stroke="#00d4ff" stroke-width="1.5" fill="rgba(0,212,255,0.06)"/>
        <rect x="14" y="14" width="12" height="12" rx="1" stroke="#00d4ff" stroke-width="1" fill="rgba(0,212,255,0.1)"/>
        <line x1="7" y1="15" x2="10" y2="15" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="7" y1="20" x2="10" y2="20" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="7" y1="25" x2="10" y2="25" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="30" y1="15" x2="33" y2="15" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="30" y1="20" x2="33" y2="20" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="30" y1="25" x2="33" y2="25" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="15" y1="7" x2="15" y2="10" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="20" y1="7" x2="20" y2="10" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="25" y1="7" x2="25" y2="10" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="15" y1="30" x2="15" y2="33" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="20" y1="30" x2="20" y2="33" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
        <line x1="25" y1="30" x2="25" y2="33" stroke="#00ff9d" stroke-width="1.5" stroke-linecap="round"/>
      </svg>
      <h1>FUCLAW SCHEDULER
        <span>TASK MANAGEMENT CONSOLE v1.0</span>
      </h1>
    </div>
    <div class="status-bar">
      <span id="connStatus"><span class="status-dot" id="connDot"></span><span id="connLabel">Offline</span></span>
      <span class="hide-sm" id="lastSync">—</span>
    </div>
  </header>

  <!-- Toolbar -->
  <div class="toolbar">
    <button class="btn btn-primary" onclick="fetchSchedule()">⬇ Load Schedule</button>
    <button class="btn btn-success" onclick="openAddModal()">＋ Add Task</button>
    <button class="btn btn-warn" onclick="clearExecuted()">✕ Clear Executed Tasks</button>
    <div class="toolbar-right">
      <button class="btn btn-success" id="saveBtn" onclick="confirmSave()" disabled>⬆ Save to Chip</button>
    </div>
  </div>

  <!-- Stats -->
  <div class="stats">
    <div class="stat-item"><span>All</span><span class="val" id="statTotal">0</span></div>
    <div class="stat-item"><span>Pending</span><span class="val" id="statPending" style="color:var(--warn)">0</span></div>
    <div class="stat-item"><span>Executed</span><span class="val" id="statDone" style="color:var(--accent2)">0</span></div>
    <div class="stat-item"><span>Recurring</span><span class="val" id="statRecur" style="color:var(--accent)">0</span></div>
  </div>

  <!-- Table -->
  <div class="table-wrap">
    <table>
      <thead>
        <tr>
          <th>#</th>
          <th>Task Description</th>
          <th>Schedule Time</th>
          <th class="hide-sm">Type</th>
          <th>Status</th>
          <th>Actions</th>
        </tr>
      </thead>
      <tbody id="tableBody">
        <tr><td colspan="6"><div class="empty-state"><div class="big">📡</div>No data loaded. Please click "Load Schedule".</div></td></tr>
      </tbody>
    </table>
  </div>
</div>

<!-- ======= Edit / Add Modal ======= -->
<div class="modal-overlay" id="editModal">
  <div class="modal">
    <div class="modal-header">
      <h2 id="modalTitle">[ Add Task ]</h2>
      <button class="modal-close" onclick="closeModal('editModal')">✕</button>
    </div>
    <div class="modal-body">
      <div class="form-row single">
        <div class="form-group">
          <label>Task Description</label>
          <input type="text" id="fTask" placeholder="Example: Turn on green light" maxlength="500">
          <small>Plain text is saved as a regular task; a valid JSON object (e.g. {"type":"tool_call",...}) is saved as structured data</small>
        </div>
      </div>

      <div class="section-label">📅 Date (enter 0 for wildcard / recurring)</div>

      <div class="form-row three">
        <div class="form-group">
          <label>Year (0 = ignore)</label>
          <input type="number" id="fYear" min="0" max="9999" value="0">
          <small>0 = Every year</small>
        </div>
        <div class="form-group">
          <label>Month (0 = ignore)</label>
          <input type="number" id="fMonth" min="0" max="12" value="0">
          <small>0 = Every month</small>
        </div>
        <div class="form-group">
          <label>Day (0 = ignore)</label>
          <input type="number" id="fDay" min="0" max="31" value="0">
          <small>0 = Every day</small>
        </div>
      </div>

      <div class="section-label">🕐 Time</div>

      <div class="form-row three">
        <div class="form-group">
          <label>Hour (0–23)</label>
          <input type="number" id="fHour" min="0" max="23" value="0">
        </div>
        <div class="form-group">
          <label>Minute (0–59)</label>
          <input type="number" id="fMinute" min="0" max="59" value="0">
        </div>
        <div class="form-group">
          <label>Second (0–59)</label>
          <input type="number" id="fSecond" min="0" max="59" value="0">
        </div>
      </div>

      <div class="toggle-row">
        <div class="toggle-wrap">
          <label class="toggle">
            <input type="checkbox" id="fExecuted">
            <span class="slider"></span>
          </label>
          <span>Executed</span>
        </div>
        <span id="executedNote" style="font-size:0.68rem;font-family:var(--mono);color:var(--text-dim)"></span>
      </div>

      <div class="hint-box" id="wildcardHint">
        ⚠ When wildcard values (0) are used in date fields, the task is treated as recurring and "Executed" is forced to false.
      </div>
    </div>
    <div class="modal-footer">
      <button class="btn" style="border-color:var(--border);color:var(--text-dim)" onclick="closeModal('editModal')">Cancel</button>
      <button class="btn btn-success" onclick="saveRow()">Confirm Save</button>
    </div>
  </div>
</div>

<!-- ======= Confirm Modal ======= -->
<div class="modal-overlay" id="confirmModal">
  <div class="modal" style="max-width:440px">
    <div class="modal-header">
      <h2 id="confirmTitle">[ Confirm Action ]</h2>
      <button class="modal-close" onclick="closeModal('confirmModal')">✕</button>
    </div>
    <div class="modal-body">
      <div class="confirm-icon" id="confirmIcon">⚠️</div>
      <div class="confirm-msg" id="confirmMsg"></div>
      <div class="confirm-detail" id="confirmDetail" style="display:none"></div>
    </div>
    <div class="modal-footer">
      <button class="btn" style="border-color:var(--border);color:var(--text-dim)" onclick="closeModal('confirmModal')">Cancel</button>
      <button class="btn btn-success" id="confirmOkBtn" onclick="confirmOk()">Confirm</button>
    </div>
  </div>
</div>

<!-- Toast container -->
<div id="toast"></div>

<!-- Loading -->
<div class="loading-overlay" id="loading">
  <div class="spinner"></div>
  <div class="loading-text" id="loadingText">PROCESSING...</div>
</div>

<script>
// ===== State =====
let schedule = [];
let editIndex = -1; // -1 = add
let pendingAction = null;
let dirty = false;

// ===== DOM helpers =====
const $ = id => document.getElementById(id);

function setDirty(val) {
  dirty = val;
  $('saveBtn').disabled = !val;
}

// ===== Format helpers =====
function isWildcard(s) {
  return s.year === 0 || s.month === 0 || s.day === 0;
}

function formatSchedule(s) {
  const dateArr = [s.year, s.month, s.day];
  const timeStr = [s.hour, s.minute, s.second].map(n => String(n).padStart(2,'0')).join(':');
  let dateParts = dateArr.map(n => n === 0 ? '<span class="wildcard">*</span>' : String(n));
  const dateStr = `<span class="date-part">${dateParts[0]}</span>/<span class="date-part">${dateParts[1]}</span>/<span class="date-part">${dateParts[2]}</span>`;
  return `<span class="time-cell">${dateStr} <span class="time-part">${timeStr}</span></span>`;
}

function scheduleType(s) {
  if (isWildcard(s)) {
    if (s.year === 0 && s.month === 0 && s.day === 0) return 'Daily';
    if (s.year === 0 && s.month === 0) return 'Monthly';
    if (s.year === 0) return 'Yearly';
    return 'Recurring';
  }
  return 'One-time';
}

function isRecurring(s) { return isWildcard(s); }

// ===== Rule: wildcard → executed must be false =====
function enforceRule(item) {
  if (isWildcard(item.schedule)) item.executed = false;
}

// ===== Render table =====
function renderTable() {
  const tbody = $('tableBody');
  if (!schedule.length) {
    tbody.innerHTML = `<tr><td colspan="6"><div class="empty-state"><div class="big">📋</div>No schedule data available</div></td></tr>`;
    updateStats(); return;
  }
  tbody.innerHTML = schedule.map((item, i) => {
    const wild = isWildcard(item.schedule);
    const typ = scheduleType(item.schedule);
    let badge = '';
    if (wild) badge = `<span class="badge badge-locked">🔁 ${typ}</span>`;
    else if (item.executed) badge = `<span class="badge badge-done">✓ Executed</span>`;
    else badge = `<span class="badge badge-pending">◉ Pending</span>`;

    return `<tr>
      <td style="font-family:var(--mono);font-size:0.7rem;color:var(--text-dim)">${i+1}</td>
      <td class="task-cell">${taskCellHtml(item.task)}</td>
      <td>${formatSchedule(item.schedule)}</td>
      <td class="hide-sm"><span style="font-family:var(--mono);font-size:0.7rem;color:var(--text-dim)">${typ}</span></td>
      <td>${badge}</td>
      <td class="actions">
        <button class="icon-btn edit" title="Edit" onclick="openEditModal(${i})">✎</button>
        <button class="icon-btn del" title="Delete" onclick="confirmDelete(${i})">✕</button>
      </td>
    </tr>`;
  }).join('');
  updateStats();
}

function updateStats() {
  $('statTotal').textContent = schedule.length;
  $('statPending').textContent = schedule.filter(x => !x.executed).length;
  $('statDone').textContent = schedule.filter(x => x.executed).length;
  $('statRecur').textContent = schedule.filter(x => isRecurring(x.schedule)).length;
}

function escHtml(s) {
  s = String(s == null ? '' : s);
  return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');
}

// 判斷使用者輸入的 task 文字是否符合 JSON 物件格式，
// 若是，存檔時保留為結構化物件（例如 tool_call）；否則存成一般字串。
function parseTaskInput(raw) {
  const trimmed = raw.trim();
  // 快速篩選：只有看起來像物件或陣列的字串才嘗試解析，避免把單純數字字串（如 "123"）誤判
  if (!(trimmed.startsWith('{') || trimmed.startsWith('['))) return raw;
  try {
    const parsed = JSON.parse(trimmed);
    if (parsed !== null && typeof parsed === 'object') return parsed;
    return raw;
  } catch (e) {
    return raw; // 不是合法 JSON，當作一般文字保留
  }
}

// task 欄位可能是字串，也可能是像 {"type":"tool_call","method":"/digitalwrite","params":{...}} 這樣的物件
function taskDisplay(task) {
  if (task == null) return '';
  if (typeof task === 'string') return task;
  if (typeof task === 'object' && task.type === 'tool_call') {
    const params = task.params ? JSON.stringify(task.params) : '';
    return `[OFFLINE] ${task.method || ''} ${params}`.trim();
  }
  try { return JSON.stringify(task); } catch (e) { return String(task); }
}

// 表格內顯示用：tool_call 的齒輪符號用較大字級呈現，其餘皆照常跳脫 HTML
function taskCellHtml(task) {
  if (task == null) return '';
  if (typeof task === 'string') return escHtml(task);
  if (typeof task === 'object' && task.type === 'tool_call') {
    const params = task.params ? JSON.stringify(task.params) : '';
    const text = `${task.method || ''} ${params}`.trim();
    return `<span class="offline-tag" title="Runs offline on the device, no cloud connection required">OFFLINE</span> ${escHtml(text)}`;
  }
  try { return escHtml(JSON.stringify(task)); } catch (e) { return escHtml(String(task)); }
}

// ===== Fetch from chip =====
async function fetchSchedule() {
  showLoading('Connecting to chip...');
  try {
    const res = await fetch('/getScheduleTasks', { method: 'GET' });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const data = await res.json();
    schedule = data;
    schedule.forEach(enforceRule);
    renderTable();
    setDirty(false);
    setConnected(true);
    toast('✓ Schedule data retrieved successfully', 'success');
    $('lastSync').textContent = 'Last sync： ' + new Date().toLocaleTimeString('zh-TW') +" [zh-TW]";
  } catch(e) {
    setConnected(false);
    toast('✗ Unable to connect to chip：' + e.message, 'error');
    // Dev fallback demo data
    if (schedule.length === 0) loadDemoData();
  } finally {
    hideLoading();
  }
}

function loadDemoData() {
  schedule = [
    {"task":"Task Description","schedule":{"year":0,"month":0,"day":0,"hour":0,"minute":0,"second":0},"executed":false}
  ];
  schedule.forEach(enforceRule);
  renderTable();
  toast('⚑ Sample data loaded (offline mode)', 'info');
}

// ===== Save to chip =====
function confirmSave() {
  const json = JSON.stringify(schedule, null, 2);
  $('confirmTitle').textContent = '[ Confirm saving to chip ]';
  $('confirmIcon').textContent = '💾';
  $('confirmMsg').innerHTML = `Proceeding to <code style="color:var(--accent)">GET /updateSchedule</code> Save <strong style="color:var(--accent2)">${schedule.length}  schedule items</strong> to the chip.<br>This operation will overwrite existing data on the chip. Do you want to continue?`;
  const detail = $('confirmDetail');
  detail.style.display = 'block';
  detail.textContent = json;
  $('confirmOkBtn').className = 'btn btn-success';
  pendingAction = 'save';
  openModal('confirmModal');
}

async function doSave() {
  showLoading('Writing to chip...');
  try {
    const json = JSON.stringify(schedule);
    // 存檔前自我驗證：確保序列化後的內容可被正確 parse 回來，避免把壞資料寫上 chip
    try {
      JSON.parse(json);
    } catch (verifyErr) {
      throw new Error('Data is not valid JSON, aborting save: ' + verifyErr.message);
    }
    const url = '/updateScheduleTasks?' + encodeURIComponent(json);
    const res = await fetch(url, { method: 'GET' });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    setDirty(false);
    setConnected(true);
    toast('✓ Schedules successfully saved to chip', 'success');
    $('lastSync').textContent = 'Last sync:' + new Date().toLocaleTimeString('zh-TW');
  } catch(e) {
    toast('✗ Save failed: ' + e.message, 'error');
  } finally {
    hideLoading();
  }
}

// ===== Add Modal =====
function openAddModal() {
  editIndex = -1;
  $('modalTitle').textContent = '[ Add Task ]';
  $('fTask').value = '';
  $('fYear').value = 0;
  $('fMonth').value = 0;
  $('fDay').value = 0;
  $('fHour').value = 8;
  $('fMinute').value = 0;
  $('fSecond').value = 0;
  $('fExecuted').checked = false;
  $('fExecuted').disabled = true;
  updateWildcardHint();
  openModal('editModal');
}

function openEditModal(i) {
  editIndex = i;
  const item = schedule[i];
  $('modalTitle').textContent = `[ Edit Schedule #${i+1} ]`;
  $('fTask').value = typeof item.task === 'string' ? item.task : JSON.stringify(item.task);
  $('fYear').value = item.schedule.year;
  $('fMonth').value = item.schedule.month;
  $('fDay').value = item.schedule.day;
  $('fHour').value = item.schedule.hour;
  $('fMinute').value = item.schedule.minute;
  $('fSecond').value = item.schedule.second;
  $('fExecuted').checked = item.executed;
  updateWildcardHint();
  openModal('editModal');
}

function updateWildcardHint() {
  const y = parseInt($('fYear').value) || 0;
  const m = parseInt($('fMonth').value) || 0;
  const d = parseInt($('fDay').value) || 0;
  const wild = y === 0 || m === 0 || d === 0;
  $('wildcardHint').classList.toggle('show', wild);
  $('fExecuted').disabled = wild;
  if (wild) $('fExecuted').checked = false;
}

['fYear','fMonth','fDay'].forEach(id => {
  $(id).addEventListener('input', updateWildcardHint);
});

function saveRow() {
  const raw = $('fTask').value.trim();
  if (!raw) { $('fTask').classList.add('invalid'); $('fTask').focus(); return; }
  $('fTask').classList.remove('invalid');

  const task = parseTaskInput(raw);

  const s = {
    year:   parseInt($('fYear').value) || 0,
    month:  parseInt($('fMonth').value) || 0,
    day:    parseInt($('fDay').value) || 0,
    hour:   parseInt($('fHour').value) || 0,
    minute: parseInt($('fMinute').value) || 0,
    second: parseInt($('fSecond').value) || 0,
  };
  const executed = $('fExecuted').checked;
  const item = { task, schedule: s, executed };
  enforceRule(item);

  if (editIndex === -1) {
    schedule.push(item);
    toast('✓ Schedule added', 'success');
  } else {
    schedule[editIndex] = item;
    toast('✓ Schedule updated #' + (editIndex+1), 'success');
  }
  setDirty(true);
  renderTable();
  closeModal('editModal');
}

// ===== Delete =====
function confirmDelete(i) {
  $('confirmTitle').textContent = '[ Confirm deletion ]';
  $('confirmIcon').textContent = '🗑️';
  $('confirmMsg').innerHTML = `This will delete schedule <strong style="color:var(--danger)"># ${i+1}</strong> ?<br><span style="color:var(--text-dim);font-size:0.8rem">"${escHtml(taskDisplay(schedule[i].task))}"</span>`;
  $('confirmDetail').style.display = 'none';
  $('confirmOkBtn').className = 'btn btn-danger';
  pendingAction = () => {
    schedule.splice(i, 1);
    setDirty(true);
    renderTable();
    toast('Schedule deleted', 'info');
  };
  openModal('confirmModal');
}

// ===== Clear executed =====
function clearExecuted() {
  const count = schedule.filter(x => x.executed && !isRecurring(x.schedule)).length;
  if (!count) { toast('No completed schedules to clear', 'info'); return; }
  $('confirmTitle').textContent = '[ Clear executed schedules ]';
  $('confirmIcon').textContent = '🧹';
  $('confirmMsg').innerHTML = `This will delete <strong style="color:var(--warn)">${count}</strong> completed one-time schedules (recurring schedules will not be affected).`;
  $('confirmDetail').style.display = 'none';
  $('confirmOkBtn').className = 'btn btn-warn';
  pendingAction = () => {
    schedule = schedule.filter(x => !(x.executed && !isRecurring(x.schedule)));
    setDirty(true);
    renderTable();
    toast(`${count} completed schedules cleared`, 'success');
  };
  openModal('confirmModal');
}

// ===== Confirm modal =====
function confirmOk() {
  closeModal('confirmModal');
  if (pendingAction === 'save') { doSave(); }
  else if (typeof pendingAction === 'function') { pendingAction(); }
  pendingAction = null;
}

// ===== Modal helpers =====
function openModal(id) { $(id).classList.add('open'); }
function closeModal(id) { $(id).classList.remove('open'); }

document.querySelectorAll('.modal-overlay').forEach(el => {
  el.addEventListener('click', e => { if (e.target === el) closeModal(el.id); });
});

// ===== Loading =====
function showLoading(msg) {
  $('loadingText').textContent = msg || 'PROCESSING...';
  $('loading').classList.add('show');
}
function hideLoading() { $('loading').classList.remove('show'); }

// ===== Toast =====
function toast(msg, type = 'info') {
  const el = document.createElement('div');
  el.className = `toast-item ${type}`;
  el.textContent = msg;
  $('toast').appendChild(el);
  setTimeout(() => el.remove(), 3200);
}

// ===== Connection status =====
function setConnected(ok) {
  $('connDot').className = 'status-dot' + (ok ? ' online' : '');
  $('connLabel').textContent = ok ? 'Connected' : 'Disconnected';
}

// ===== Init =====
renderTable();
</script>

<!-- fuClaw Global Nav Bar -->
<style>
  .fc-nav {
    position: fixed; bottom: 0; left: 0; right: 0; z-index: 9999;
    background: rgba(255,255,255,.92);
    backdrop-filter: blur(16px);
    -webkit-backdrop-filter: blur(16px);
    border-top: 1px solid rgba(220,228,255,.9);
    display: flex; justify-content: space-around; align-items: stretch;
    height: 58px;
    box-shadow: 0 -4px 24px rgba(79,110,247,.10);
    font-family: -apple-system,'Segoe UI','Noto Sans TC',sans-serif;
  }
  .fc-nav a {
    flex: 1; display: flex; flex-direction: column;
    align-items: center; justify-content: center;
    gap: 3px; text-decoration: none;
    color: #8892b0; font-size: .6rem; font-weight: 500;
    letter-spacing: .03em; transition: color .2s;
    -webkit-tap-highlight-color: transparent;
    padding: 6px 2px 4px;
  }
  .fc-nav a:hover { color: #4f6ef7; }
  .fc-nav a.fc-active { color: #4f6ef7; }
  .fc-nav a .fc-ico {
    width: 28px; height: 28px; border-radius: 9px;
    display: flex; align-items: center; justify-content: center;
    font-size: 1.05rem; transition: background .2s, transform .15s;
  }
  .fc-nav a.fc-active .fc-ico { background: rgba(79,110,247,.12); transform: scale(1.08); }
  .fc-nav a:hover .fc-ico { background: rgba(79,110,247,.07); }
  body { padding-bottom: 68px !important; }
</style>
<nav class="fc-nav">
  <a href="/"           id="fcn-config"   title="System Config"><span class="fc-ico">&#9881;</span><span>Config</span></a>
  <a href="/agent"    id="fcn-agent"    title="Agent Config"><span class="fc-ico">&#129302;</span><span>Agent</span></a>
  <a href="/schedule"  id="fcn-schedule" title="Scheduler"><span class="fc-ico">&#128197;</span><span>Schedule</span></a>
  <a href="/chat"      id="fcn-chat"     title="Gemini Chat"><span class="fc-ico">&#128172;</span><span>Chat</span></a>
</nav>
<script>
  (function(){
    var map={"/":"fcn-config","":"fcn-config","/agent":"fcn-agent",
             "/schedule":"fcn-schedule","/chat":"fcn-chat"};
    var seg=location.pathname.replace(/\/$/,"") || "/";
    var el=document.getElementById(map[seg]); if(el) el.className+=" fc-active";
  })();
</script>

</body>
</html>
)rawhtml";

#endif
