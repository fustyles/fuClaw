#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char INDEX_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>fuClaw AI Agent — System Settings</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@400;500;600;700&family=Noto+Sans+TC:wght@300;400;500&family=JetBrains+Mono:wght@400;500&display=swap');

  :root {
    --bg:        #f0f4ff;
    --surface:   #ffffff;
    --surface2:  #f7f9ff;
    --border:    #dde3f5;
    --border2:   #c4cef5;
    --accent:    #4f6ef7;
    --accent2:   #7c3aed;
    --accent3:   #06b6d4;
    --text:      #1a1d2e;
    --text2:     #4a5270;
    --text3:     #8892b0;
    --success:   #10b981;
    --warn:      #f59e0b;
    --danger:    #ef4444;
    --radius:    12px;
  }

  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    background: var(--bg);
    background-image:
      radial-gradient(circle at 20% 10%, rgba(79,110,247,.08) 0%, transparent 50%),
      radial-gradient(circle at 80% 80%, rgba(124,58,237,.07) 0%, transparent 50%),
      radial-gradient(circle at 60% 30%, rgba(6,182,212,.06) 0%, transparent 40%);
    color: var(--text);
    font-family: 'Noto Sans TC', 'Space Grotesk', sans-serif;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 40px 16px 60px;
  }

  .wrapper {
    width: 100%;
    max-width: 660px;
  }

  /* ── header ── */
  header {
    text-align: center;
    margin-bottom: 36px;
  }

  .logo-row {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 14px;
    margin-bottom: 8px;
  }

  .logo-icon {
    width: 52px; height: 52px;
    background: linear-gradient(135deg, var(--accent), var(--accent2));
    border-radius: 16px;
    display: flex; align-items: center; justify-content: center;
    box-shadow: 0 8px 24px rgba(79,110,247,.3);
    position: relative;
    overflow: hidden;
    flex-shrink: 0;
  }
  .logo-icon::before {
    content: '';
    position: absolute;
    top: -50%; left: -50%;
    width: 200%; height: 200%;
    background: conic-gradient(transparent 30%, rgba(255,255,255,.15) 50%, transparent 70%);
    animation: spin 4s linear infinite;
  }
  @keyframes spin { to { transform: rotate(360deg); } }
  .logo-icon svg { position: relative; z-index: 1; }

  .logo-mark {
    font-family: 'Space Grotesk', sans-serif;
    font-weight: 700;
    font-size: 2.2rem;
    letter-spacing: -.02em;
    background: linear-gradient(135deg, var(--accent) 0%, var(--accent2) 100%);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
  }

  .tagline {
    font-size: .8rem;
    color: var(--text3);
    letter-spacing: .18em;
    text-transform: uppercase;
    margin-top: 4px;
  }

  .chips {
    display: flex;
    gap: 8px;
    justify-content: center;
    margin-top: 14px;
    flex-wrap: wrap;
  }
  .chip {
    display: inline-flex;
    align-items: center;
    gap: 5px;
    padding: 4px 12px;
    background: white;
    border: 1px solid var(--border);
    border-radius: 20px;
    font-size: .72rem;
    color: var(--text2);
    box-shadow: 0 1px 4px rgba(0,0,0,.06);
  }
  .chip-dot {
    width: 6px; height: 6px;
    border-radius: 50%;
    background: var(--success);
    animation: blink 2s ease-in-out infinite;
  }
  @keyframes blink {
    0%,100% { opacity: 1; } 50% { opacity: .3; }
  }

  /* ── card ── */
  .card {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 20px;
    padding: 36px 36px;
    box-shadow: 0 4px 24px rgba(79,110,247,.08), 0 1px 4px rgba(0,0,0,.06);
  }

  /* section label */
  .section-label {
    display: flex;
    align-items: center;
    gap: 10px;
    margin: 28px 0 16px;
    font-size: .72rem;
    font-weight: 600;
    letter-spacing: .12em;
    text-transform: uppercase;
    color: var(--text3);
  }
  .section-label:first-of-type { margin-top: 0; }
  .section-label .sl-icon {
    width: 26px; height: 26px;
    border-radius: 8px;
    display: flex; align-items: center; justify-content: center;
    font-size: .88rem;
    flex-shrink: 0;
  }
  .sl-blue  { background: rgba(79,110,247,.1);  color: var(--accent); }
  .sl-purple{ background: rgba(124,58,237,.1);  color: var(--accent2); }
  .sl-cyan  { background: rgba(6,182,212,.1);   color: var(--accent3); }
  .sl-green { background: rgba(16,185,129,.1);  color: var(--success); }
  .section-label::after {
    content: '';
    flex: 1;
    height: 1px;
    background: var(--border);
  }

  /* ── field ── */
  .field { margin-bottom: 16px; }
  .field:last-of-type { margin-bottom: 0; }

  .field-header {
    display: flex;
    align-items: baseline;
    gap: 8px;
    margin-bottom: 6px;
  }
  label {
    font-size: .82rem;
    font-weight: 500;
    color: var(--text);
    letter-spacing: .01em;
  }
  .field-hint {
    font-size: .7rem;
    color: var(--text3);
  }

  .input-wrap { position: relative; }

  .input-wrap .icon {
    position: absolute;
    left: 13px;
    top: 50%;
    transform: translateY(-50%);
    font-size: 1rem;
    pointer-events: none;
    color: var(--text3);
    transition: color .2s;
  }

  input[type="text"],
  input[type="password"] {
    width: 100%;
    padding: 10px 40px 10px 40px;
    background: var(--surface2);
    border: 1.5px solid var(--border);
    border-radius: var(--radius);
    color: var(--text);
    font-family: 'JetBrains Mono', monospace;
    font-size: .84rem;
    outline: none;
    transition: border-color .2s, background .2s, box-shadow .2s;
    -webkit-appearance: none;
  }
  input::placeholder { color: var(--text3); font-size: .8rem; }
  input:focus {
    border-color: var(--accent);
    background: white;
    box-shadow: 0 0 0 3px rgba(79,110,247,.12);
  }
  input:focus ~ .icon { color: var(--accent); }

  .toggle-vis {
    position: absolute;
    right: 10px;
    top: 50%;
    transform: translateY(-50%);
    background: none;
    border: none;
    color: var(--text3);
    cursor: pointer;
    font-size: .9rem;
    padding: 4px 6px;
    transition: color .2s;
    line-height: 1;
  }
  .toggle-vis:hover { color: var(--accent); }

  /* select */
  .select-wrap { position: relative; }
  select {
    width: 100%;
    padding: 10px 36px 10px 40px;
    background: var(--surface2);
    border: 1.5px solid var(--border);
    border-radius: var(--radius);
    color: var(--text);
    font-family: 'Noto Sans TC', sans-serif;
    font-size: .84rem;
    outline: none;
    cursor: pointer;
    -webkit-appearance: none;
    appearance: none;
    transition: border-color .2s, box-shadow .2s;
  }
  select:focus {
    border-color: var(--accent);
    box-shadow: 0 0 0 3px rgba(79,110,247,.12);
  }
  select option { background: white; color: var(--text); }
  .select-arrow {
    position: absolute;
    right: 12px;
    top: 50%;
    transform: translateY(-50%);
    pointer-events: none;
    color: var(--text3);
    font-size: .75rem;
  }
  .select-icon {
    position: absolute;
    left: 13px;
    top: 50%;
    transform: translateY(-50%);
    pointer-events: none;
    color: var(--text3);
    font-size: 1rem;
  }

  /* ── submit ── */
  .btn-row { margin-top: 32px; }

  .btn-submit {
    width: 100%;
    padding: 14px;
    background: linear-gradient(135deg, var(--accent), var(--accent2));
    border: none;
    border-radius: var(--radius);
    color: white;
    font-family: 'Space Grotesk', sans-serif;
    font-size: .9rem;
    font-weight: 600;
    letter-spacing: .04em;
    cursor: pointer;
    position: relative;
    overflow: hidden;
    transition: transform .15s, box-shadow .15s, opacity .15s;
    box-shadow: 0 6px 20px rgba(79,110,247,.35);
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 8px;
  }
  .btn-submit:hover {
    transform: translateY(-2px);
    box-shadow: 0 10px 30px rgba(79,110,247,.45);
  }
  .btn-submit:active { transform: translateY(0); opacity: .9; }
  .btn-submit .arrow { font-size: 1.1rem; }

  /* ── toast ── */
  .toast {
    display: none;
    margin-top: 16px;
    padding: 12px 16px;
    border-radius: var(--radius);
    font-size: .82rem;
    letter-spacing: .02em;
    text-align: center;
  }
  .toast.success {
    display: block;
    background: rgba(16,185,129,.08);
    border: 1px solid rgba(16,185,129,.25);
    color: #059669;
  }
  .toast.error {
    display: block;
    background: rgba(239,68,68,.07);
    border: 1px solid rgba(239,68,68,.25);
    color: #dc2626;
  }

  /* ── json preview ── */
  .json-preview {
    display: none;
    margin-top: 16px;
    padding: 16px;
    background: #1a1d2e;
    border-radius: var(--radius);
    font-family: 'JetBrains Mono', monospace;
    font-size: .75rem;
    color: #a5d6a7;
    line-height: 1.8;
    white-space: pre;
    overflow-x: auto;
  }
  .json-key   { color: #90caf9; }
  .json-str   { color: #a5d6a7; }
  .json-punct { color: #b0bec5; }

  /* ── footer ── */
  footer {
    margin-top: 24px;
    text-align: center;
    font-size: .68rem;
    color: var(--text3);
    letter-spacing: .06em;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 10px;
  }
  footer::before, footer::after {
    content: '';
    width: 32px;
    height: 1px;
    background: var(--border);
  }

  @media (max-width: 480px) {
    .card { padding: 24px 18px; }
  }
</style>
</head>
<body>


<a href="https://github.com/fustyles/fuClaw">https://github.com/fustyles/fuClaw</a>

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
  <a href="/"         id="fcn-config"   title="System Config"><span class="fc-ico">&#9881;</span><span>Home</span></a>
  <a href="/schedule" id="fcn-schedule" title="Scheduler"><span class="fc-ico">&#128197;</span><span>Schedule</span></a>
  <a href="/chat"     id="fcn-chat"     title="Gemini Chat"><span class="fc-ico">&#128172;</span><span>Chat</span></a>
  <a href="/mqtt" id="fcn-mqtt"     title="MQTT Chat"><span class="fc-ico">&#128225;</span><span>MQTT</span></a>
</nav>
<script>
  (function(){
    var map={"/":"fcn-config","":"fcn-config",
             "/schedule":"fcn-schedule","/chat":"fcn-chat",
             "/mqtt":"fcn-mqtt"};
    var seg=location.pathname.replace(/\/$/,"") || "/";
    var el=document.getElementById(map[seg]); if(el) el.className+=" fc-active";
  })();
</script>

</body>
</html>
)rawhtml";

#endif