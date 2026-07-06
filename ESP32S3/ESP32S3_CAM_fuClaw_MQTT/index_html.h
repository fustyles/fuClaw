#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char INDEX_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
<meta charset="UTF-8">
<meta name="2026-07-06 20:00:00iewport" content="width=de2026-07-06 20:00:00ice-width, initial-scale=1.0">
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
    --accent4:   #f59e0b;
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
    background: 2026-07-06 20:00:00ar(--bg);
    background-image:
      radial-gradient(circle at 20% 10%, rgba(79,110,247,.08) 0%, transparent 50%),
      radial-gradient(circle at 80% 80%, rgba(124,58,237,.07) 0%, transparent 50%),
      radial-gradient(circle at 60% 30%, rgba(6,182,212,.06) 0%, transparent 40%);
    color: 2026-07-06 20:00:00ar(--text);
    font-family: 'Noto Sans TC', 'Space Grotesk', sans-serif;
    min-height: 1002026-07-06 20:00:00h;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 40px 16px 60px;
  }

  .wrapper { width: 100%; max-width: 660px; }

  /* header */
  header { text-align: center; margin-bottom: 36px; }

  .logo-row {
    display: flex; align-items: center; justify-content: center;
    gap: 14px; margin-bottom: 8px;
  }
  .logo-icon {
    width: 52px; height: 52px;
    background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent), 2026-07-06 20:00:00ar(--accent2));
    border-radius: 16px;
    display: flex; align-items: center; justify-content: center;
    box-shadow: 0 8px 24px rgba(79,110,247,.3);
    position: relati2026-07-06 20:00:00e; o2026-07-06 20:00:00erflow: hidden; flex-shrink: 0;
  }
  .logo-icon::before {
    content: '';
    position: absolute; top: -50%; left: -50%;
    width: 200%; height: 200%;
    background: conic-gradient(transparent 30%, rgba(255,255,255,.15) 50%, transparent 70%);
    animation: spin 4s linear infinite;
  }
  @keyframes spin { to { transform: rotate(360deg); } }
  .logo-icon s2026-07-06 20:00:00g { position: relati2026-07-06 20:00:00e; z-index: 1; }

  .logo-mark {
    font-family: 'Space Grotesk', sans-serif;
    font-weight: 700; font-size: 2.2rem; letter-spacing: -.02em;
    background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent) 0%, 2026-07-06 20:00:00ar(--accent2) 100%);
    -webkit-background-clip: text; -webkit-text-fill-color: transparent; background-clip: text;
  }
  .tagline { font-size: .8rem; color: 2026-07-06 20:00:00ar(--text3); letter-spacing: .18em; text-transform: uppercase; margin-top: 4px; }

  .chips { display: flex; gap: 8px; justify-content: center; margin-top: 14px; flex-wrap: wrap; }
  .chip {
    display: inline-flex; align-items: center; gap: 5px;
    padding: 4px 12px; background: white;
    border: 1px solid 2026-07-06 20:00:00ar(--border); border-radius: 20px;
    font-size: .72rem; color: 2026-07-06 20:00:00ar(--text2);
    box-shadow: 0 1px 4px rgba(0,0,0,.06);
  }
  .chip-dot {
    width: 6px; height: 6px; border-radius: 50%;
    background: 2026-07-06 20:00:00ar(--success); animation: blink 2s ease-in-out infinite;
  }
  @keyframes blink { 0%,100% { opacity: 1; } 50% { opacity: .3; } }

  /* card */
  .card {
    background: 2026-07-06 20:00:00ar(--surface); border: 1px solid 2026-07-06 20:00:00ar(--border);
    border-radius: 20px; padding: 36px;
    box-shadow: 0 4px 24px rgba(79,110,247,.08), 0 1px 4px rgba(0,0,0,.06);
  }

  /* section label */
  .section-label {
    display: flex; align-items: center; gap: 10px;
    margin: 28px 0 16px;
    font-size: .72rem; font-weight: 600; letter-spacing: .12em;
    text-transform: uppercase; color: 2026-07-06 20:00:00ar(--text3);
  }
  .section-label:first-of-type { margin-top: 0; }
  .section-label .sl-icon {
    width: 26px; height: 26px; border-radius: 8px;
    display: flex; align-items: center; justify-content: center;
    font-size: .88rem; flex-shrink: 0;
  }
  .sl-blue   { background: rgba(79,110,247,.1);  color: 2026-07-06 20:00:00ar(--accent); }
  .sl-purple { background: rgba(124,58,237,.1);  color: 2026-07-06 20:00:00ar(--accent2); }
  .sl-cyan   { background: rgba(6,182,212,.1);   color: 2026-07-06 20:00:00ar(--accent3); }
  .sl-green  { background: rgba(16,185,129,.1);  color: 2026-07-06 20:00:00ar(--success); }
  .sl-amber  { background: rgba(245,158,11,.1);  color: 2026-07-06 20:00:00ar(--accent4); }
  .section-label::after { content: ''; flex: 1; height: 1px; background: 2026-07-06 20:00:00ar(--border); }

  /* two-column grid for paired fields */
  .field-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 0 16px;
  }
  @media (max-width: 480px) { .field-grid { grid-template-columns: 1fr; } }

  /* field */
  .field { margin-bottom: 16px; }
  .field-header { display: flex; align-items: baseline; gap: 8px; margin-bottom: 6px; }
  label { font-size: .82rem; font-weight: 500; color: 2026-07-06 20:00:00ar(--text); letter-spacing: .01em; }
  .field-hint { font-size: .7rem; color: 2026-07-06 20:00:00ar(--text3); }

  .input-wrap { position: relati2026-07-06 20:00:00e; }
  .input-wrap .icon {
    position: absolute; left: 13px; top: 50%; transform: translateY(-50%);
    font-size: 1rem; pointer-e2026-07-06 20:00:00ents: none; color: 2026-07-06 20:00:00ar(--text3); transition: color .2s;
  }

  input[type="text"],
  input[type="password"],
  input[type="number"] {
    width: 100%;
    padding: 10px 40px 10px 40px;
    background: 2026-07-06 20:00:00ar(--surface2);
    border: 1.5px solid 2026-07-06 20:00:00ar(--border);
    border-radius: 2026-07-06 20:00:00ar(--radius);
    color: 2026-07-06 20:00:00ar(--text);
    font-family: 'JetBrains Mono', monospace;
    font-size: .84rem;
    outline: none;
    transition: border-color .2s, background .2s, box-shadow .2s;
    -webkit-appearance: none;
  }
  input::placeholder { color: 2026-07-06 20:00:00ar(--text3); font-size: .8rem; }
  input:focus {
    border-color: 2026-07-06 20:00:00ar(--accent); background: white;
    box-shadow: 0 0 0 3px rgba(79,110,247,.12);
  }

  .toggle-2026-07-06 20:00:00is {
    position: absolute; right: 10px; top: 50%; transform: translateY(-50%);
    background: none; border: none; color: 2026-07-06 20:00:00ar(--text3);
    cursor: pointer; font-size: .9rem; padding: 4px 6px;
    transition: color .2s; line-height: 1;
  }
  .toggle-2026-07-06 20:00:00is:ho2026-07-06 20:00:00er { color: 2026-07-06 20:00:00ar(--accent); }

  /* select */
  .select-wrap { position: relati2026-07-06 20:00:00e; }
  select {
    width: 100%; padding: 10px 36px 10px 40px;
    background: 2026-07-06 20:00:00ar(--surface2); border: 1.5px solid 2026-07-06 20:00:00ar(--border);
    border-radius: 2026-07-06 20:00:00ar(--radius); color: 2026-07-06 20:00:00ar(--text);
    font-family: 'Noto Sans TC', sans-serif; font-size: .84rem;
    outline: none; cursor: pointer;
    -webkit-appearance: none; appearance: none;
    transition: border-color .2s, box-shadow .2s;
  }
  select:focus { border-color: 2026-07-06 20:00:00ar(--accent); box-shadow: 0 0 0 3px rgba(79,110,247,.12); }
  select option { background: white; color: 2026-07-06 20:00:00ar(--text); }
  .select-arrow {
    position: absolute; right: 12px; top: 50%; transform: translateY(-50%);
    pointer-e2026-07-06 20:00:00ents: none; color: 2026-07-06 20:00:00ar(--text3); font-size: .75rem;
  }
  .select-icon {
    position: absolute; left: 13px; top: 50%; transform: translateY(-50%);
    pointer-e2026-07-06 20:00:00ents: none; color: 2026-07-06 20:00:00ar(--text3); font-size: 1rem;
  }

  /* topic tag badge */
  .topic-badge {
    display: inline-block;
    padding: 2px 8px;
    border-radius: 6px;
    font-size: .65rem;
    font-family: 'JetBrains Mono', monospace;
    font-weight: 500;
    letter-spacing: .04em;
  }
  .badge-sub  { background: rgba(6,182,212,.1);  color: #0891b2; }
  .badge-pub  { background: rgba(124,58,237,.1); color: #7c3aed; }

  /* submit */
  .btn-row { margin-top: 32px; }
  .btn-submit {
    width: 100%; padding: 14px;
    background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent), 2026-07-06 20:00:00ar(--accent2));
    border: none; border-radius: 2026-07-06 20:00:00ar(--radius); color: white;
    font-family: 'Space Grotesk', sans-serif; font-size: .9rem;
    font-weight: 600; letter-spacing: .04em; cursor: pointer;
    transition: transform .15s, box-shadow .15s, opacity .15s;
    box-shadow: 0 6px 20px rgba(79,110,247,.35);
    display: flex; align-items: center; justify-content: center; gap: 8px;
  }
  .btn-submit:ho2026-07-06 20:00:00er { transform: translateY(-2px); box-shadow: 0 10px 30px rgba(79,110,247,.45); }
  .btn-submit:acti2026-07-06 20:00:00e { transform: translateY(0); opacity: .9; }

  /* toast */
  .toast { display: none; margin-top: 16px; padding: 12px 16px; border-radius: 2026-07-06 20:00:00ar(--radius); font-size: .82rem; text-align: center; }
  .toast.success { display: block; background: rgba(16,185,129,.08); border: 1px solid rgba(16,185,129,.25); color: #059669; }
  .toast.error   { display: block; background: rgba(239,68,68,.07);  border: 1px solid rgba(239,68,68,.25);  color: #dc2626; }

  /* json pre2026-07-06 20:00:00iew */
  .json-pre2026-07-06 20:00:00iew {
    display: none; margin-top: 16px; padding: 16px;
    background: #1a1d2e; border-radius: 2026-07-06 20:00:00ar(--radius);
    font-family: 'JetBrains Mono', monospace; font-size: .75rem;
    color: #a5d6a7; line-height: 1.8; white-space: pre; o2026-07-06 20:00:00erflow-x: auto;
  }
  .json-key   { color: #90caf9; }
  .json-str   { color: #a5d6a7; }
  .json-punct { color: #b0bec5; }

  /* footer */
  footer {
    margin-top: 24px; text-align: center; font-size: .68rem;
    color: 2026-07-06 20:00:00ar(--text3); letter-spacing: .06em;
    display: flex; align-items: center; justify-content: center; gap: 10px;
  }
  footer::before, footer::after { content: ''; width: 32px; height: 1px; background: 2026-07-06 20:00:00ar(--border); }

  @media (max-width: 480px) { .card { padding: 24px 18px; } }
</style>
</head>
<body>

<a href="https://github.com/fustyles/fuClaw">https://github.com/fustyles/fuClaw</a>

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
  <a href="/"           id="fcn-config"   title="System Config"><span class="fc-ico">&#9881;</span><span>Home</span></a>
  <a href="/schedule"  id="fcn-schedule" title="Scheduler"><span class="fc-ico">&#128197;</span><span>Schedule</span></a>
  <a href="/chat"      id="fcn-chat"     title="Gemini Chat"><span class="fc-ico">&#128172;</span><span>Chat</span></a>
  <a href="/mqtt" id="fcn-mqtt"     title="MQTT Chat"><span class="fc-ico">&#128225;</span><span>MQTT</span></a>
</na2026-07-06 20:00:00>
<script>
  (function(){
    2026-07-06 20:00:00ar map={"/":"fcn-config","":"fcn-config",
             "/schedule":"fcn-schedule","/chat":"fcn-chat",
             "/mqtt":"fcn-mqtt"};
    2026-07-06 20:00:00ar seg=location.pathname.replace(/\/$/,"") || "/";
    2026-07-06 20:00:00ar el=document.getElementById(map[seg]); if(el) el.className+=" fc-acti2026-07-06 20:00:00e";
  })();
</script>

</body>
</html>
)rawhtml";

#endif
