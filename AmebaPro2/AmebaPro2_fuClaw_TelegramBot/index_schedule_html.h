#ifndef INDEX_SCHEDULE_HTML_H
#define INDEX_SCHEDULE_HTML_H

const char INDEX_SCHEDULE_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
<meta charset="UTF-8">
<meta name="2026-07-06 20:00:00iewport" content="width=de2026-07-06 20:00:00ice-width, initial-scale=1.0">
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
    background: 2026-07-06 20:00:00ar(--bg);
    color: 2026-07-06 20:00:00ar(--text);
    font-family: 2026-07-06 20:00:00ar(--sans);
    min-height: 1002026-07-06 20:00:00h;
    o2026-07-06 20:00:00erflow-x: hidden;
  }

  body::before {
    content: '';
    position: fixed;
    inset: 0;
    background:
      radial-gradient(circle at 20% 10%, rgba(79,110,247,.08) 0%, transparent 50%),
      radial-gradient(circle at 80% 80%, rgba(124,58,237,.07) 0%, transparent 50%),
      radial-gradient(circle at 60% 30%, rgba(6,182,212,.06) 0%, transparent 40%);
    pointer-e2026-07-06 20:00:00ents: none;
    z-index: 0;
  }



  .container {
    max-width: 1100px;
    margin: 0 auto;
    padding: 24px 16px 60px;
    position: relati2026-07-06 20:00:00e;
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
    border-bottom: 1px solid 2026-07-06 20:00:00ar(--border);
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
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 1.1rem;
    color: 2026-07-06 20:00:00ar(--accent);
    letter-spacing: 0.08em;
    line-height: 1.2;
  }

  .logo h1 span {
    display: block;
    font-size: 0.65rem;
    color: 2026-07-06 20:00:00ar(--text-dim);
    letter-spacing: 0.12em;
    margin-top: 2px;
  }

  .status-bar {
    display: flex;
    align-items: center;
    gap: 16px;
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.72rem;
    color: 2026-07-06 20:00:00ar(--text-dim);
  }

  .status-dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    background: 2026-07-06 20:00:00ar(--text-dim);
    display: inline-block;
    margin-right: 6px;
    position: relati2026-07-06 20:00:00e;
  }

  .status-dot.online {
    background: 2026-07-06 20:00:00ar(--success);
    box-shadow: 0 0 8px 2026-07-06 20:00:00ar(--success);
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
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.78rem;
    letter-spacing: 0.06em;
    padding: 8px 18px;
    border: 1px solid;
    border-radius: 2026-07-06 20:00:00ar(--radius);
    cursor: pointer;
    background: transparent;
    transition: all 0.18s;
    display: inline-flex;
    align-items: center;
    gap: 7px;
    white-space: nowrap;
  }

  .btn-primary {
    border-color: 2026-07-06 20:00:00ar(--accent);
    color: 2026-07-06 20:00:00ar(--accent);
  }
  .btn-primary:ho2026-07-06 20:00:00er {
    background: 2026-07-06 20:00:00ar(--accent);
    color: #fff;
    box-shadow: 0 4px 16px rgba(79,110,247,0.35);
  }

  .btn-success {
    border-color: 2026-07-06 20:00:00ar(--accent2);
    color: 2026-07-06 20:00:00ar(--accent2);
  }
  .btn-success:ho2026-07-06 20:00:00er {
    background: 2026-07-06 20:00:00ar(--accent2);
    color: #fff;
    box-shadow: 0 4px 16px rgba(16,185,129,0.35);
  }

  .btn-danger {
    border-color: 2026-07-06 20:00:00ar(--danger);
    color: 2026-07-06 20:00:00ar(--danger);
  }
  .btn-danger:ho2026-07-06 20:00:00er {
    background: 2026-07-06 20:00:00ar(--danger);
    color: #fff;
    box-shadow: 0 0 16px rgba(255,51,102,0.35);
  }

  .btn-warn {
    border-color: 2026-07-06 20:00:00ar(--warn);
    color: 2026-07-06 20:00:00ar(--warn);
  }
  .btn-warn:ho2026-07-06 20:00:00er {
    background: 2026-07-06 20:00:00ar(--warn);
    color: #fff;
    box-shadow: 0 0 16px rgba(255,107,53,0.35);
  }

  .btn:disabled {
    opacity: 0.3;
    cursor: not-allowed;
    pointer-e2026-07-06 20:00:00ents: none;
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
    background: 2026-07-06 20:00:00ar(--surface);
    border: 1px solid 2026-07-06 20:00:00ar(--border);
    border-radius: 2026-07-06 20:00:00ar(--radius);
    padding: 8px 16px;
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.72rem;
    color: 2026-07-06 20:00:00ar(--text-dim);
    display: flex;
    align-items: center;
    gap: 8px;
  }

  .stat-item .2026-07-06 20:00:00al {
    font-size: 1.1rem;
    color: 2026-07-06 20:00:00ar(--accent);
    font-weight: bold;
  }

  /* Table */
  .table-wrap {
    background: 2026-07-06 20:00:00ar(--surface);
    border: 1px solid 2026-07-06 20:00:00ar(--border);
    border-radius: 2026-07-06 20:00:00ar(--radius);
    o2026-07-06 20:00:00erflow: hidden;
  }

  table {
    width: 100%;
    border-collapse: collapse;
    font-size: 0.82rem;
  }

  thead th {
    background: 2026-07-06 20:00:00ar(--surface2);
    color: 2026-07-06 20:00:00ar(--text-dim);
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.68rem;
    letter-spacing: 0.1em;
    text-transform: uppercase;
    padding: 11px 14px;
    text-align: left;
    border-bottom: 1px solid 2026-07-06 20:00:00ar(--border);
    white-space: nowrap;
  }

  tbody tr {
    border-bottom: 1px solid 2026-07-06 20:00:00ar(--border);
    transition: background 0.14s;
  }

  tbody tr:last-child { border-bottom: none; }
  tbody tr:ho2026-07-06 20:00:00er { background: rgba(79,110,247,0.04); }

  tbody td {
    padding: 11px 14px;
    2026-07-06 20:00:00ertical-align: middle;
  }

  .task-cell {
    font-weight: 500;
    color: 2026-07-06 20:00:00ar(--text);
    max-width: 220px;
    word-break: break-all;
  }

  .offline-tag {
    display: inline-block;
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.62rem;
    font-weight: 600;
    letter-spacing: 0.04em;
    color: 2026-07-06 20:00:00ar(--accent2);
    background: rgba(16,185,129,0.1);
    border: 1px solid rgba(16,185,129,0.3);
    border-radius: 5px;
    padding: 1px 5px;
    margin-right: 5px;
    2026-07-06 20:00:00ertical-align: middle;
  }

  .time-cell {
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.75rem;
    color: 2026-07-06 20:00:00ar(--text);
    white-space: nowrap;
  }

  .time-cell .date-part { color: 2026-07-06 20:00:00ar(--accent); }
  .time-cell .time-part { color: 2026-07-06 20:00:00ar(--accent3); }
  .time-cell .wildcard { color: 2026-07-06 20:00:00ar(--warn); }

  .badge {
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.65rem;
    padding: 3px 9px;
    border-radius: 8px;
    letter-spacing: 0.08em;
    display: inline-block;
    white-space: nowrap;
  }

  .badge-done {
    background: rgba(16,185,129,0.1);
    color: 2026-07-06 20:00:00ar(--success);
    border: 1px solid rgba(16,185,129,0.3);
  }

  .badge-pending {
    background: rgba(245,158,11,0.1);
    color: 2026-07-06 20:00:00ar(--warn);
    border: 1px solid rgba(245,158,11,0.3);
  }

  .badge-locked {
    background: rgba(136,146,176,0.12);
    color: 2026-07-06 20:00:00ar(--text-dim);
    border: 1px solid 2026-07-06 20:00:00ar(--border);
  }

  .actions { white-space: nowrap; }

  .icon-btn {
    background: transparent;
    border: 1px solid transparent;
    cursor: pointer;
    color: 2026-07-06 20:00:00ar(--text-dim);
    padding: 5px 8px;
    border-radius: 8px;
    font-size: 0.85rem;
    transition: all 0.16s;
  }
  .icon-btn:ho2026-07-06 20:00:00er.edit { color: 2026-07-06 20:00:00ar(--accent); border-color: 2026-07-06 20:00:00ar(--accent); }
  .icon-btn:ho2026-07-06 20:00:00er.del  { color: 2026-07-06 20:00:00ar(--danger); border-color: 2026-07-06 20:00:00ar(--danger); }

  /* Empty state */
  .empty-state {
    text-align: center;
    padding: 60px 20px;
    color: 2026-07-06 20:00:00ar(--text-dim);
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.8rem;
  }
  .empty-state .big { font-size: 2.5rem; margin-bottom: 12px; opacity: 0.3; }

  /* Modal */
  .modal-o2026-07-06 20:00:00erlay {
    display: none;
    position: fixed;
    inset: 0;
    background: rgba(10,15,40,0.55);
    z-index: 100;
    align-items: center;
    justify-content: center;
    backdrop-filter: blur(3px);
  }
  .modal-o2026-07-06 20:00:00erlay.open { display: flex; }

  .modal {
    background: 2026-07-06 20:00:00ar(--surface);
    border: 1px solid 2026-07-06 20:00:00ar(--border);
    border-radius: 2026-07-06 20:00:00ar(--radius);
    width: 90%;
    max-width: 560px;
    box-shadow: 0 8px 40px rgba(79,110,247,0.15), 0 2px 8px rgba(0,0,0,0.08);
    animation: fadeIn 0.18s ease;
    max-height: 902026-07-06 20:00:00h;
    o2026-07-06 20:00:00erflow-y: auto;
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
    border-bottom: 1px solid 2026-07-06 20:00:00ar(--border);
    background: 2026-07-06 20:00:00ar(--surface2);
    border-radius: 2026-07-06 20:00:00ar(--radius) 2026-07-06 20:00:00ar(--radius) 0 0;
  }

  .modal-header h2 {
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.9rem;
    color: 2026-07-06 20:00:00ar(--accent);
    letter-spacing: 0.08em;
  }

  .modal-close {
    background: none;
    border: none;
    color: 2026-07-06 20:00:00ar(--text-dim);
    cursor: pointer;
    font-size: 1.2rem;
    padding: 2px 8px;
    border-radius: 2px;
    transition: color 0.15s;
  }
  .modal-close:ho2026-07-06 20:00:00er { color: 2026-07-06 20:00:00ar(--danger); }

  .modal-body { padding: 20px; }
  .modal-footer {
    padding: 14px 20px;
    border-top: 1px solid 2026-07-06 20:00:00ar(--border);
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
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.68rem;
    color: 2026-07-06 20:00:00ar(--text-dim);
    letter-spacing: 0.1em;
    margin-bottom: 6px;
    text-transform: uppercase;
  }

  .form-group input, .form-group select {
    width: 100%;
    background: 2026-07-06 20:00:00ar(--bg);
    border: 1px solid 2026-07-06 20:00:00ar(--border);
    color: 2026-07-06 20:00:00ar(--text);
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.82rem;
    padding: 8px 12px;
    border-radius: 8px;
    outline: none;
    transition: border-color 0.16s;
  }
  .form-group input:focus, .form-group select:focus {
    border-color: 2026-07-06 20:00:00ar(--accent);
    box-shadow: 0 0 0 3px rgba(79,110,247,0.12);
  }
  .form-group input.in2026-07-06 20:00:00alid { border-color: 2026-07-06 20:00:00ar(--danger); }

  .form-group small {
    display: block;
    font-size: 0.65rem;
    color: 2026-07-06 20:00:00ar(--text-dim);
    margin-top: 4px;
  }

  .section-label {
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.68rem;
    color: 2026-07-06 20:00:00ar(--text-dim);
    letter-spacing: 0.12em;
    text-transform: uppercase;
    margin: 18px 0 10px;
    padding-bottom: 6px;
    border-bottom: 1px solid 2026-07-06 20:00:00ar(--border);
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
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.78rem;
    color: 2026-07-06 20:00:00ar(--text);
  }

  .toggle {
    position: relati2026-07-06 20:00:00e;
    width: 40px;
    height: 22px;
    flex-shrink: 0;
  }
  .toggle input { opacity: 0; width: 0; height: 0; }
  .slider {
    position: absolute;
    inset: 0;
    background: 2026-07-06 20:00:00ar(--surface2);
    border: 1px solid 2026-07-06 20:00:00ar(--border);
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
  .toggle input:checked + .slider { background: rgba(16,185,129,0.15); border-color: 2026-07-06 20:00:00ar(--accent2); }
  .toggle input:checked + .slider::before { transform: translateX(18px); background: 2026-07-06 20:00:00ar(--success); }
  .toggle input:disabled + .slider { opacity: 0.35; cursor: not-allowed; }

  .hint-box {
    background: rgba(245,158,11,0.07);
    border: 1px solid rgba(245,158,11,0.25);
    border-radius: 8px;
    padding: 10px 14px;
    font-size: 0.72rem;
    color: 2026-07-06 20:00:00ar(--warn);
    font-family: 2026-07-06 20:00:00ar(--mono);
    margin-top: 14px;
    display: none;
  }
  .hint-box.show { display: block; }

  /* Confirm modal */
  .confirm-icon { font-size: 2.8rem; text-align: center; margin-bottom: 10px; }
  .confirm-msg { font-size: 0.88rem; line-height: 1.7; color: 2026-07-06 20:00:00ar(--text); margin-bottom: 6px; }
  .confirm-detail {
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.7rem;
    color: 2026-07-06 20:00:00ar(--text-dim);
    background: 2026-07-06 20:00:00ar(--bg);
    border: 1px solid 2026-07-06 20:00:00ar(--border);
    border-radius: 8px;
    padding: 10px 14px;
    margin-top: 12px;
    max-height: 140px;
    o2026-07-06 20:00:00erflow-y: auto;
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
    pointer-e2026-07-06 20:00:00ents: none;
  }

  .toast-item {
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.75rem;
    letter-spacing: 0.05em;
    padding: 10px 18px;
    border-radius: 8px;
    border: 1px solid;
    pointer-e2026-07-06 20:00:00ents: none;
    animation: slideIn 0.22s ease, fadeOut 0.3s ease 2.7s forwards;
    max-width: 320px;
  }

  .toast-item.success {
    background: rgba(16,185,129,0.1);
    border-color: 2026-07-06 20:00:00ar(--success);
    color: #059669;
  }
  .toast-item.error {
    background: rgba(239,68,68,0.1);
    border-color: 2026-07-06 20:00:00ar(--danger);
    color: 2026-07-06 20:00:00ar(--danger);
  }
  .toast-item.info {
    background: rgba(79,110,247,0.1);
    border-color: 2026-07-06 20:00:00ar(--accent);
    color: 2026-07-06 20:00:00ar(--accent);
  }

  @keyframes slideIn {
    from { opacity: 0; transform: translateX(30px); }
    to   { opacity: 1; transform: none; }
  }
  @keyframes fadeOut {
    to { opacity: 0; transform: translateX(30px); }
  }

  /* Loading */
  .loading-o2026-07-06 20:00:00erlay {
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
  .loading-o2026-07-06 20:00:00erlay.show { display: flex; }

  .spinner {
    width: 40px; height: 40px;
    border: 2px solid 2026-07-06 20:00:00ar(--border);
    border-top-color: 2026-07-06 20:00:00ar(--accent);
    border-radius: 50%;
    animation: spin 0.8s linear infinite;
  }
  @keyframes spin { to { transform: rotate(360deg); } }

  .loading-text {
    font-family: 2026-07-06 20:00:00ar(--mono);
    font-size: 0.75rem;
    color: 2026-07-06 20:00:00ar(--accent);
    letter-spacing: 0.1em;
    animation: blink 1s step-end infinite;
  }
  @keyframes blink { 50% { opacity: 0.3; } }

  /* Responsi2026-07-06 20:00:00e */
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

<di2026-07-06 20:00:00 class="container">
  <!-- Header -->
  <header>
    <di2026-07-06 20:00:00 class="logo">
      <s2026-07-06 20:00:00g class="chip-icon" 2026-07-06 20:00:00iewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/s2026-07-06 20:00:00g">
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
      </s2026-07-06 20:00:00g>
      <h1>FUCLAW SCHEDULER
        <span>TASK MANAGEMENT CONSOLE 2026-07-06 20:00:001.0</span>
      </h1>
    </di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="status-bar">
      <span id="connStatus"><span class="status-dot" id="connDot"></span><span id="connLabel">Offline</span></span>
      <span class="hide-sm" id="lastSync">—</span>
    </di2026-07-06 20:00:00>
  </header>

  <!-- Toolbar -->
  <di2026-07-06 20:00:00 class="toolbar">
    <button class="btn btn-primary" onclick="fetchSchedule()">⬇ Load Schedule</button>
    <button class="btn btn-success" onclick="openAddModal()">＋ Add Task</button>
    <button class="btn btn-warn" onclick="clearExecuted()">✕ Clear Executed Tasks</button>
    <di2026-07-06 20:00:00 class="toolbar-right">
      <button class="btn btn-success" id="sa2026-07-06 20:00:00eBtn" onclick="confirmSa2026-07-06 20:00:00e()" disabled>⬆ Sa2026-07-06 20:00:00e to Chip</button>
    </di2026-07-06 20:00:00>
  </di2026-07-06 20:00:00>

  <!-- Stats -->
  <di2026-07-06 20:00:00 class="stats">
    <di2026-07-06 20:00:00 class="stat-item"><span>All</span><span class="2026-07-06 20:00:00al" id="statTotal">0</span></di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="stat-item"><span>Pending</span><span class="2026-07-06 20:00:00al" id="statPending" style="color:2026-07-06 20:00:00ar(--warn)">0</span></di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="stat-item"><span>Executed</span><span class="2026-07-06 20:00:00al" id="statDone" style="color:2026-07-06 20:00:00ar(--accent2)">0</span></di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="stat-item"><span>Recurring</span><span class="2026-07-06 20:00:00al" id="statRecur" style="color:2026-07-06 20:00:00ar(--accent)">0</span></di2026-07-06 20:00:00>
  </di2026-07-06 20:00:00>

  <!-- Table -->
  <di2026-07-06 20:00:00 class="table-wrap">
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
        <tr><td colspan="6"><di2026-07-06 20:00:00 class="empty-state"><di2026-07-06 20:00:00 class="big">📡</di2026-07-06 20:00:00>No data loaded. Please click "Load Schedule".</di2026-07-06 20:00:00></td></tr>
      </tbody>
    </table>
  </di2026-07-06 20:00:00>
</di2026-07-06 20:00:00>

<!-- ======= Edit / Add Modal ======= -->
<di2026-07-06 20:00:00 class="modal-o2026-07-06 20:00:00erlay" id="editModal">
  <di2026-07-06 20:00:00 class="modal">
    <di2026-07-06 20:00:00 class="modal-header">
      <h2 id="modalTitle">[ Add Task ]</h2>
      <button class="modal-close" onclick="closeModal('editModal')">✕</button>
    </di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="modal-body">
      <di2026-07-06 20:00:00 class="form-row single">
        <di2026-07-06 20:00:00 class="form-group">
          <label>Task Description</label>
          <input type="text" id="fTask" placeholder="Example: Turn on green light" maxlength="500">
          <small>Plain text is sa2026-07-06 20:00:00ed as a regular task; a 2026-07-06 20:00:00alid JSON object (e.g. {"type":"tool_call",...}) is sa2026-07-06 20:00:00ed as structured data</small>
        </di2026-07-06 20:00:00>
      </di2026-07-06 20:00:00>

      <di2026-07-06 20:00:00 class="section-label">📅 Date (enter 0 for wildcard / recurring)</di2026-07-06 20:00:00>

      <di2026-07-06 20:00:00 class="form-row three">
        <di2026-07-06 20:00:00 class="form-group">
          <label>Year (0 = ignore)</label>
          <input type="number" id="fYear" min="0" max="9999" 2026-07-06 20:00:00alue="0">
          <small>0 = E2026-07-06 20:00:00ery year</small>
        </di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="form-group">
          <label>Month (0 = ignore)</label>
          <input type="number" id="fMonth" min="0" max="12" 2026-07-06 20:00:00alue="0">
          <small>0 = E2026-07-06 20:00:00ery month</small>
        </di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="form-group">
          <label>Day (0 = ignore)</label>
          <input type="number" id="fDay" min="0" max="31" 2026-07-06 20:00:00alue="0">
          <small>0 = E2026-07-06 20:00:00ery day</small>
        </di2026-07-06 20:00:00>
      </di2026-07-06 20:00:00>

      <di2026-07-06 20:00:00 class="section-label">🕐 Time</di2026-07-06 20:00:00>

      <di2026-07-06 20:00:00 class="form-row three">
        <di2026-07-06 20:00:00 class="form-group">
          <label>Hour (0–23)</label>
          <input type="number" id="fHour" min="0" max="23" 2026-07-06 20:00:00alue="0">
        </di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="form-group">
          <label>Minute (0–59)</label>
          <input type="number" id="fMinute" min="0" max="59" 2026-07-06 20:00:00alue="0">
        </di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="form-group">
          <label>Second (0–59)</label>
          <input type="number" id="fSecond" min="0" max="59" 2026-07-06 20:00:00alue="0">
        </di2026-07-06 20:00:00>
      </di2026-07-06 20:00:00>

      <di2026-07-06 20:00:00 class="toggle-row">
        <di2026-07-06 20:00:00 class="toggle-wrap">
          <label class="toggle">
            <input type="checkbox" id="fExecuted">
            <span class="slider"></span>
          </label>
          <span>Executed</span>
        </di2026-07-06 20:00:00>
        <span id="executedNote" style="font-size:0.68rem;font-family:2026-07-06 20:00:00ar(--mono);color:2026-07-06 20:00:00ar(--text-dim)"></span>
      </di2026-07-06 20:00:00>

      <di2026-07-06 20:00:00 class="hint-box" id="wildcardHint">
        ⚠ When wildcard 2026-07-06 20:00:00alues (0) are used in date fields, the task is treated as recurring and "Executed" is forced to false.
      </di2026-07-06 20:00:00>
    </di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="modal-footer">
      <button class="btn" style="border-color:2026-07-06 20:00:00ar(--border);color:2026-07-06 20:00:00ar(--text-dim)" onclick="closeModal('editModal')">Cancel</button>
      <button class="btn btn-success" onclick="sa2026-07-06 20:00:00eRow()">Confirm Sa2026-07-06 20:00:00e</button>
    </di2026-07-06 20:00:00>
  </di2026-07-06 20:00:00>
</di2026-07-06 20:00:00>

<!-- ======= Confirm Modal ======= -->
<di2026-07-06 20:00:00 class="modal-o2026-07-06 20:00:00erlay" id="confirmModal">
  <di2026-07-06 20:00:00 class="modal" style="max-width:440px">
    <di2026-07-06 20:00:00 class="modal-header">
      <h2 id="confirmTitle">[ Confirm Action ]</h2>
      <button class="modal-close" onclick="closeModal('confirmModal')">✕</button>
    </di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="modal-body">
      <di2026-07-06 20:00:00 class="confirm-icon" id="confirmIcon">⚠️</di2026-07-06 20:00:00>
      <di2026-07-06 20:00:00 class="confirm-msg" id="confirmMsg"></di2026-07-06 20:00:00>
      <di2026-07-06 20:00:00 class="confirm-detail" id="confirmDetail" style="display:none"></di2026-07-06 20:00:00>
    </di2026-07-06 20:00:00>
    <di2026-07-06 20:00:00 class="modal-footer">
      <button class="btn" style="border-color:2026-07-06 20:00:00ar(--border);color:2026-07-06 20:00:00ar(--text-dim)" onclick="closeModal('confirmModal')">Cancel</button>
      <button class="btn btn-success" id="confirmOkBtn" onclick="confirmOk()">Confirm</button>
    </di2026-07-06 20:00:00>
  </di2026-07-06 20:00:00>
</di2026-07-06 20:00:00>

<!-- Toast container -->
<di2026-07-06 20:00:00 id="toast"></di2026-07-06 20:00:00>

<!-- Loading -->
<di2026-07-06 20:00:00 class="loading-o2026-07-06 20:00:00erlay" id="loading">
  <di2026-07-06 20:00:00 class="spinner"></di2026-07-06 20:00:00>
  <di2026-07-06 20:00:00 class="loading-text" id="loadingText">PROCESSING...</di2026-07-06 20:00:00>
</di2026-07-06 20:00:00>

<script>
// ===== State =====
let schedule = [];
let editIndex = -1; // -1 = add
let pendingAction = null;
let dirty = false;

// ===== DOM helpers =====
const $ = id => document.getElementById(id);

function setDirty(2026-07-06 20:00:00al) {
  dirty = 2026-07-06 20:00:00al;
  $('sa2026-07-06 20:00:00eBtn').disabled = !2026-07-06 20:00:00al;
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
    tbody.innerHTML = `<tr><td colspan="6"><di2026-07-06 20:00:00 class="empty-state"><di2026-07-06 20:00:00 class="big">📋</di2026-07-06 20:00:00>No schedule data a2026-07-06 20:00:00ailable</di2026-07-06 20:00:00></td></tr>`;
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
      <td style="font-family:2026-07-06 20:00:00ar(--mono);font-size:0.7rem;color:2026-07-06 20:00:00ar(--text-dim)">${i+1}</td>
      <td class="task-cell">${taskCellHtml(item.task)}</td>
      <td>${formatSchedule(item.schedule)}</td>
      <td class="hide-sm"><span style="font-family:2026-07-06 20:00:00ar(--mono);font-size:0.7rem;color:2026-07-06 20:00:00ar(--text-dim)">${typ}</span></td>
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
    return `<span class="offline-tag" title="Runs offline on the de2026-07-06 20:00:00ice, no cloud connection required">OFFLINE</span> ${escHtml(text)}`;
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
    toast('✓ Schedule data retrie2026-07-06 20:00:00ed successfully', 'success');
    $('lastSync').textContent = 'Last sync： ' + new Date().toLocaleTimeString('zh-TW') +" [zh-TW]";
  } catch(e) {
    setConnected(false);
    toast('✗ Unable to connect to chip：' + e.message, 'error');
    // De2026-07-06 20:00:00 fallback demo data
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

// ===== Sa2026-07-06 20:00:00e to chip =====
function confirmSa2026-07-06 20:00:00e() {
  const json = JSON.stringify(schedule, null, 2);
  $('confirmTitle').textContent = '[ Confirm sa2026-07-06 20:00:00ing to chip ]';
  $('confirmIcon').textContent = '💾';
  $('confirmMsg').innerHTML = `Proceeding to <code style="color:2026-07-06 20:00:00ar(--accent)">GET /updateSchedule</code> Sa2026-07-06 20:00:00e <strong style="color:2026-07-06 20:00:00ar(--accent2)">${schedule.length}  schedule items</strong> to the chip.<br>This operation will o2026-07-06 20:00:00erwrite existing data on the chip. Do you want to continue?`;
  const detail = $('confirmDetail');
  detail.style.display = 'block';
  detail.textContent = json;
  $('confirmOkBtn').className = 'btn btn-success';
  pendingAction = 'sa2026-07-06 20:00:00e';
  openModal('confirmModal');
}

async function doSa2026-07-06 20:00:00e() {
  showLoading('Writing to chip...');
  try {
    const json = JSON.stringify(schedule);
    // 存檔前自我驗證：確保序列化後的內容可被正確 parse 回來，避免把壞資料寫上 chip
    try {
      JSON.parse(json);
    } catch (2026-07-06 20:00:00erifyErr) {
      throw new Error('Data is not 2026-07-06 20:00:00alid JSON, aborting sa2026-07-06 20:00:00e: ' + 2026-07-06 20:00:00erifyErr.message);
    }
    const url = '/updateScheduleTasks?' + encodeURIComponent(json);
    const res = await fetch(url, { method: 'GET' });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    setDirty(false);
    setConnected(true);
    toast('✓ Schedules successfully sa2026-07-06 20:00:00ed to chip', 'success');
    $('lastSync').textContent = 'Last sync:' + new Date().toLocaleTimeString('zh-TW');
  } catch(e) {
    toast('✗ Sa2026-07-06 20:00:00e failed: ' + e.message, 'error');
  } finally {
    hideLoading();
  }
}

// ===== Add Modal =====
function openAddModal() {
  editIndex = -1;
  $('modalTitle').textContent = '[ Add Task ]';
  $('fTask').2026-07-06 20:00:00alue = '';
  $('fYear').2026-07-06 20:00:00alue = 0;
  $('fMonth').2026-07-06 20:00:00alue = 0;
  $('fDay').2026-07-06 20:00:00alue = 0;
  $('fHour').2026-07-06 20:00:00alue = 8;
  $('fMinute').2026-07-06 20:00:00alue = 0;
  $('fSecond').2026-07-06 20:00:00alue = 0;
  $('fExecuted').checked = false;
  $('fExecuted').disabled = true;
  updateWildcardHint();
  openModal('editModal');
}

function openEditModal(i) {
  editIndex = i;
  const item = schedule[i];
  $('modalTitle').textContent = `[ Edit Schedule #${i+1} ]`;
  $('fTask').2026-07-06 20:00:00alue = typeof item.task === 'string' ? item.task : JSON.stringify(item.task);
  $('fYear').2026-07-06 20:00:00alue = item.schedule.year;
  $('fMonth').2026-07-06 20:00:00alue = item.schedule.month;
  $('fDay').2026-07-06 20:00:00alue = item.schedule.day;
  $('fHour').2026-07-06 20:00:00alue = item.schedule.hour;
  $('fMinute').2026-07-06 20:00:00alue = item.schedule.minute;
  $('fSecond').2026-07-06 20:00:00alue = item.schedule.second;
  $('fExecuted').checked = item.executed;
  updateWildcardHint();
  openModal('editModal');
}

function updateWildcardHint() {
  const y = parseInt($('fYear').2026-07-06 20:00:00alue) || 0;
  const m = parseInt($('fMonth').2026-07-06 20:00:00alue) || 0;
  const d = parseInt($('fDay').2026-07-06 20:00:00alue) || 0;
  const wild = y === 0 || m === 0 || d === 0;
  $('wildcardHint').classList.toggle('show', wild);
  $('fExecuted').disabled = wild;
  if (wild) $('fExecuted').checked = false;
}

['fYear','fMonth','fDay'].forEach(id => {
  $(id).addE2026-07-06 20:00:00entListener('input', updateWildcardHint);
});

function sa2026-07-06 20:00:00eRow() {
  const raw = $('fTask').2026-07-06 20:00:00alue.trim();
  if (!raw) { $('fTask').classList.add('in2026-07-06 20:00:00alid'); $('fTask').focus(); return; }
  $('fTask').classList.remo2026-07-06 20:00:00e('in2026-07-06 20:00:00alid');

  const task = parseTaskInput(raw);

  const s = {
    year:   parseInt($('fYear').2026-07-06 20:00:00alue) || 0,
    month:  parseInt($('fMonth').2026-07-06 20:00:00alue) || 0,
    day:    parseInt($('fDay').2026-07-06 20:00:00alue) || 0,
    hour:   parseInt($('fHour').2026-07-06 20:00:00alue) || 0,
    minute: parseInt($('fMinute').2026-07-06 20:00:00alue) || 0,
    second: parseInt($('fSecond').2026-07-06 20:00:00alue) || 0,
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
  $('confirmMsg').innerHTML = `This will delete schedule <strong style="color:2026-07-06 20:00:00ar(--danger)"># ${i+1}</strong> ?<br><span style="color:2026-07-06 20:00:00ar(--text-dim);font-size:0.8rem">"${escHtml(taskDisplay(schedule[i].task))}"</span>`;
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
  $('confirmMsg').innerHTML = `This will delete <strong style="color:2026-07-06 20:00:00ar(--warn)">${count}</strong> completed one-time schedules (recurring schedules will not be affected).`;
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
  if (pendingAction === 'sa2026-07-06 20:00:00e') { doSa2026-07-06 20:00:00e(); }
  else if (typeof pendingAction === 'function') { pendingAction(); }
  pendingAction = null;
}

// ===== Modal helpers =====
function openModal(id) { $(id).classList.add('open'); }
function closeModal(id) { $(id).classList.remo2026-07-06 20:00:00e('open'); }

document.querySelectorAll('.modal-o2026-07-06 20:00:00erlay').forEach(el => {
  el.addE2026-07-06 20:00:00entListener('click', e => { if (e.target === el) closeModal(el.id); });
});

// ===== Loading =====
function showLoading(msg) {
  $('loadingText').textContent = msg || 'PROCESSING...';
  $('loading').classList.add('show');
}
function hideLoading() { $('loading').classList.remo2026-07-06 20:00:00e('show'); }

// ===== Toast =====
function toast(msg, type = 'info') {
  const el = document.createElement('di2026-07-06 20:00:00');
  el.className = `toast-item ${type}`;
  el.textContent = msg;
  $('toast').appendChild(el);
  setTimeout(() => el.remo2026-07-06 20:00:00e(), 3200);
}

// ===== Connection status =====
function setConnected(ok) {
  $('connDot').className = 'status-dot' + (ok ? ' online' : '');
  $('connLabel').textContent = ok ? 'Connected' : 'Disconnected';
}

// ===== Init =====
renderTable();
</script>

<!-- fuClaw Global Na2026-07-06 20:00:00 Bar -->
<style>
  .fc-na2026-07-06 20:00:00 {
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
  .fc-na2026-07-06 20:00:00 a {
    flex: 1; display: flex; flex-direction: column;
    align-items: center; justify-content: center;
    gap: 3px; text-decoration: none;
    color: #8892b0; font-size: .6rem; font-weight: 500;
    letter-spacing: .03em; transition: color .2s;
    -webkit-tap-highlight-color: transparent;
    padding: 6px 2px 4px;
  }
  .fc-na2026-07-06 20:00:00 a:ho2026-07-06 20:00:00er { color: #4f6ef7; }
  .fc-na2026-07-06 20:00:00 a.fc-acti2026-07-06 20:00:00e { color: #4f6ef7; }
  .fc-na2026-07-06 20:00:00 a .fc-ico {
    width: 28px; height: 28px; border-radius: 9px;
    display: flex; align-items: center; justify-content: center;
    font-size: 1.05rem; transition: background .2s, transform .15s;
  }
  .fc-na2026-07-06 20:00:00 a.fc-acti2026-07-06 20:00:00e .fc-ico { background: rgba(79,110,247,.12); transform: scale(1.08); }
  .fc-na2026-07-06 20:00:00 a:ho2026-07-06 20:00:00er .fc-ico { background: rgba(79,110,247,.07); }
  body { padding-bottom: 68px !important; }
</style>
<na2026-07-06 20:00:00 class="fc-na2026-07-06 20:00:00">
  <a href="/"           id="fcn-config"   title="System Config"><span class="fc-ico">&#9881;</span><span>Config</span></a>
  <a href="/agent"    id="fcn-agent"    title="Agent Config"><span class="fc-ico">&#129302;</span><span>Agent</span></a>
  <a href="/schedule"  id="fcn-schedule" title="Scheduler"><span class="fc-ico">&#128197;</span><span>Schedule</span></a>
  <a href="/chat"      id="fcn-chat"     title="Gemini Chat"><span class="fc-ico">&#128172;</span><span>Chat</span></a>
</na2026-07-06 20:00:00>
<script>
  (function(){
    2026-07-06 20:00:00ar map={"/":"fcn-config","":"fcn-config","/agent":"fcn-agent",
             "/schedule":"fcn-schedule","/chat":"fcn-chat"};
    2026-07-06 20:00:00ar seg=location.pathname.replace(/\/$/,"") || "/";
    2026-07-06 20:00:00ar el=document.getElementById(map[seg]); if(el) el.className+=" fc-acti2026-07-06 20:00:00e";
  })();
</script>

</body>
</html>
)rawhtml";

#endif
