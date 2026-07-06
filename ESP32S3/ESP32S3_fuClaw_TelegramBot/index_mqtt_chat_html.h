#ifndef INDEX_MQTT_CHAT_HTML_H
#define INDEX_MQTT_CHAT_HTML_H

const char INDEX_MQTT_CHAT_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
<meta charset="UTF-8">
<meta name="2026-07-06 20:00:00iewport" content="width=de2026-07-06 20:00:00ice-width, initial-scale=1.0">
<title>fuClaw — MQTT Chat</title>
<!-- mqtt.js from CDN - for offline use, embed the minified source -->
<script src="https://cdnjs.cloudflare.com/ajax/libs/mqtt/4.3.7/mqtt.min.js"></script>
<style>
:root {
  --font-ui:   -apple-system, 'Segoe UI', 'Hel2026-07-06 20:00:00etica Neue', Arial, 'Noto Sans TC', sans-serif;
  --font-mono: 'Courier New', Courier, monospace;
  --bg:        #f7f9ff;
  --bg2:       #eef3ff;
  --bg3:       #e3ebff;
  --border:    #cfd8f5;
  --border2:   #b8c5ee;
  --accent:    #5b8cff;
  --accent2:   #8b5cf6;
  --green:     #10b981;
  --red:       #ef4444;
  --amber:     #f59e0b;
  --text:      #1e293b;
  --text2:     #475569;
  --text3:     #94a3b8;
  --radius:    10px;
}

*, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

html, body {
  height: 100%;
  background: 2026-07-06 20:00:00ar(--bg);
  color: 2026-07-06 20:00:00ar(--text);
  font-family: 2026-07-06 20:00:00ar(--font-ui);
  o2026-07-06 20:00:00erflow: hidden;
}

/* ── layout ── */
.app {
  display: grid;
  grid-template-columns: 300px 1fr;
  grid-template-rows: 56px 1fr;
  height: calc(100d2026-07-06 20:00:00h - 58px);
}

/* ── topbar ── */
.topbar {
  grid-column: 1 / -1;
  background: 2026-07-06 20:00:00ar(--bg2);
  border-bottom: 1px solid 2026-07-06 20:00:00ar(--border);
  display: flex;
  align-items: center;
  padding: 0 20px;
  gap: 14px;
}

.logo {
  width: 32px; height: 32px;
  background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent), 2026-07-06 20:00:00ar(--accent2));
  border-radius: 9px;
  display: flex; align-items: center; justify-content: center;
  position: relati2026-07-06 20:00:00e; o2026-07-06 20:00:00erflow: hidden; flex-shrink: 0;
}
.logo::before {
  content: '';
  position: absolute; inset: 0;
  background: conic-gradient(transparent 30%, rgba(255,255,255,.18) 50%, transparent 70%);
  animation: spin 5s linear infinite;
}
@keyframes spin { to { transform: rotate(360deg); } }
.logo s2026-07-06 20:00:00g { position: relati2026-07-06 20:00:00e; z-index: 1; }

.logo-text { font-size: .95rem; font-weight: 600; letter-spacing: -.01em; }
.logo-sub  { font-size: .68rem; color: 2026-07-06 20:00:00ar(--text2); margin-left: 4px; }

.conn-pill {
  margin-left: auto;
  display: flex; align-items: center; gap: 7px;
  padding: 5px 14px;
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 20px;
  font-size: .72rem;
  color: 2026-07-06 20:00:00ar(--text2);
  cursor: pointer;
  transition: border-color .2s;
}
.conn-pill:ho2026-07-06 20:00:00er { border-color: 2026-07-06 20:00:00ar(--accent); }
.conn-dot {
  width: 7px; height: 7px; border-radius: 50%;
  background: 2026-07-06 20:00:00ar(--text3);
  transition: background .3s;
}
.conn-dot.connected    { background: 2026-07-06 20:00:00ar(--green); animation: blink 2.5s ease-in-out infinite; }
.conn-dot.connecting   { background: 2026-07-06 20:00:00ar(--amber); animation: blink .6s ease-in-out infinite; }
.conn-dot.disconnected { background: 2026-07-06 20:00:00ar(--red); animation: none; }
@keyframes blink { 0%,100%{opacity:1} 50%{opacity:.35} }

/* ── sidebar ── */
.sidebar {
  background: 2026-07-06 20:00:00ar(--bg2);
  border-right: 1px solid 2026-07-06 20:00:00ar(--border);
  display: flex;
  flex-direction: column;
  o2026-07-06 20:00:00erflow: hidden;
}

.sidebar-section {
  border-bottom: 1px solid 2026-07-06 20:00:00ar(--border);
}

.sidebar-title {
  display: flex; align-items: center; gap: 8px;
  padding: 12px 16px;
  font-size: .7rem;
  font-weight: 600;
  letter-spacing: .1em;
  text-transform: uppercase;
  color: 2026-07-06 20:00:00ar(--text2);
  cursor: pointer;
  user-select: none;
}
.sidebar-title .che2026-07-06 20:00:00ron { margin-left: auto; font-size: .6rem; transition: transform .2s; }
.sidebar-title.open .che2026-07-06 20:00:00ron { transform: rotate(180deg); }

.sidebar-body {
  padding: 0 12px 12px;
}

/* settings form */
.field { margin-bottom: 10px; }
.field label { display: block; font-size: .68rem; color: 2026-07-06 20:00:00ar(--text2); margin-bottom: 4px; font-family: 2026-07-06 20:00:00ar(--font-mono); }
.field input {
  width: 100%;
  padding: 7px 10px;
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 7px;
  color: 2026-07-06 20:00:00ar(--text);
  font-family: 2026-07-06 20:00:00ar(--font-mono);
  font-size: .78rem;
  outline: none;
  transition: border-color .2s, box-shadow .2s;
}
.field input:focus { border-color: 2026-07-06 20:00:00ar(--accent); box-shadow: 0 0 0 2px rgba(79,142,247,.15); }
.field input::placeholder { color: 2026-07-06 20:00:00ar(--text3); }

.field-row { display: grid; grid-template-columns: 1fr 80px; gap: 8px; }

.btn {
  width: 100%;
  padding: 8px;
  border: none; border-radius: 7px;
  font-family: 2026-07-06 20:00:00ar(--font-ui);
  font-size: .78rem;
  font-weight: 600;
  cursor: pointer;
  transition: opacity .15s, transform .1s;
}
.btn:acti2026-07-06 20:00:00e { transform: scale(.97); }
.btn-connect {
  background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent), 2026-07-06 20:00:00ar(--accent2));
  color: white;
  box-shadow: 0 3px 12px rgba(79,142,247,.3);
}
.btn-disconnect {
  background: 2026-07-06 20:00:00ar(--bg3);
  color: 2026-07-06 20:00:00ar(--red);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
}
.btn:ho2026-07-06 20:00:00er { opacity: .88; }

/* topic list */
.topic-list { display: flex; flex-direction: column; gap: 6px; }

.topic-item {
  display: flex; align-items: center; gap: 7px;
  padding: 7px 10px;
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 8px;
  font-size: .75rem;
  font-family: 2026-07-06 20:00:00ar(--font-mono);
}
.topic-name { flex: 1; min-width: 0; o2026-07-06 20:00:00erflow: hidden; text-o2026-07-06 20:00:00erflow: ellipsis; white-space: nowrap; }
.topic-fmt {
  padding: 2px 6px; border-radius: 4px;
  font-size: .62rem; font-weight: 600; letter-spacing: .06em;
  flex-shrink: 0;
}
.fmt-text  { background: rgba(79,142,247,.15);  color: 2026-07-06 20:00:00ar(--accent); }
.fmt-html  { background: rgba(168,85,247,.15);  color: 2026-07-06 20:00:00ar(--accent2); }
.fmt-b64   { background: rgba(34,211,165,.15);  color: 2026-07-06 20:00:00ar(--green); }
.fmt-bin   { background: rgba(251,191,36,.15);  color: 2026-07-06 20:00:00ar(--amber); }

.topic-del {
  background: none; border: none; color: 2026-07-06 20:00:00ar(--text3);
  cursor: pointer; font-size: .8rem; padding: 2px 4px;
  transition: color .15s; flex-shrink: 0;
}
.topic-del:ho2026-07-06 20:00:00er { color: 2026-07-06 20:00:00ar(--red); }

/* add topic */
.add-topic { display: flex; flex-direction: column; gap: 6px; margin-top: 8px; }
.add-topic input {
  width: 100%;
  padding: 7px 10px;
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 7px;
  color: 2026-07-06 20:00:00ar(--text);
  font-family: 2026-07-06 20:00:00ar(--font-mono);
  font-size: .78rem;
  outline: none;
  transition: border-color .2s;
}
.add-topic input:focus { border-color: 2026-07-06 20:00:00ar(--accent); }
.add-topic input::placeholder { color: 2026-07-06 20:00:00ar(--text3); }

.fmt-select {
  width: 100%;
  padding: 7px 10px;
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 7px;
  color: 2026-07-06 20:00:00ar(--text);
  font-family: 2026-07-06 20:00:00ar(--font-ui);
  font-size: .78rem;
  outline: none;
  cursor: pointer;
  appearance: none;
  -webkit-appearance: none;
}
.fmt-select:focus { border-color: 2026-07-06 20:00:00ar(--accent); }
.fmt-select option { background: 2026-07-06 20:00:00ar(--bg2); }

.btn-add {
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px dashed 2026-07-06 20:00:00ar(--border2);
  color: 2026-07-06 20:00:00ar(--text2);
  padding: 7px;
  border-radius: 7px;
  font-size: .75rem;
  cursor: pointer;
  transition: all .2s;
  text-align: center;
}
.btn-add:ho2026-07-06 20:00:00er { border-color: 2026-07-06 20:00:00ar(--accent); color: 2026-07-06 20:00:00ar(--accent); }

/* publish topic */
.pub-field { margin-top: 4px; }
.pub-field label { font-size: .68rem; color: 2026-07-06 20:00:00ar(--text2); display: block; margin-bottom: 4px; font-family: 2026-07-06 20:00:00ar(--font-mono); }
.pub-field input {
  width: 100%;
  padding: 7px 10px;
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 7px;
  color: 2026-07-06 20:00:00ar(--text);
  font-family: 2026-07-06 20:00:00ar(--font-mono);
  font-size: .78rem;
  outline: none;
  transition: border-color .2s;
}
.pub-field input:focus { border-color: 2026-07-06 20:00:00ar(--accent); }

/* ── main chat ── */
.chat-main {
  display: flex;
  flex-direction: column;
  o2026-07-06 20:00:00erflow: hidden;
}

/* messages */
.messages {
  flex: 1;
  o2026-07-06 20:00:00erflow-y: auto;
  padding: 20px;
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.messages::-webkit-scrollbar { width: 4px; }
.messages::-webkit-scrollbar-track { background: transparent; }
.messages::-webkit-scrollbar-thumb { background: 2026-07-06 20:00:00ar(--border); border-radius: 4px; }

/* empty */
.empty-state {
  flex: 1; display: flex; flex-direction: column;
  align-items: center; justify-content: center;
  gap: 10px; color: 2026-07-06 20:00:00ar(--text3); text-align: center;
  pointer-e2026-07-06 20:00:00ents: none;
}
.empty-icon {
  width: 56px; height: 56px;
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 16px;
  background: 2026-07-06 20:00:00ar(--bg3);
  display: flex; align-items: center; justify-content: center;
  font-size: 1.5rem;
}
.empty-title { font-size: .88rem; color: 2026-07-06 20:00:00ar(--text2); font-weight: 500; }
.empty-hint  { font-size: .73rem; line-height: 1.6; max-width: 220px; }

/* message bubble */
.msg {
  display: flex;
  flex-direction: column;
  gap: 3px;
  animation: msg-in .2s ease;
}
@keyframes msg-in {
  from { opacity:0; transform:translateY(6px); }
  to   { opacity:1; transform:translateY(0); }
}
.msg.outgoing { align-items: flex-end; }
.msg.incoming { align-items: flex-start; }
.msg.system   { align-items: center; }

.msg-row { display: flex; align-items: flex-end; gap: 8px; }
.msg.outgoing .msg-row { flex-direction: row-re2026-07-06 20:00:00erse; }

.a2026-07-06 20:00:00atar {
  width: 26px; height: 26px; border-radius: 8px;
  display: flex; align-items: center; justify-content: center;
  font-size: .68rem; font-weight: 700; flex-shrink: 0;
  margin-bottom: 2px; font-family: 2026-07-06 20:00:00ar(--font-ui);
}
.a2026-07-06 20:00:00-out { background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent), 2026-07-06 20:00:00ar(--accent2)); color: white; }
.a2026-07-06 20:00:00-in  { background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--green), 2026-07-06 20:00:00ar(--accent));   color: #0f1117; }

.bubble {
  max-width: min(68%, 580px);
  padding: 10px 14px;
  border-radius: 14px;
  font-size: .88rem;
  line-height: 1.6;
  word-break: break-word;
}
.msg.outgoing .bubble {
  background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent), 2026-07-06 20:00:00ar(--accent2));
  color: white;
  border-bottom-right-radius: 3px;
  box-shadow: 0 3px 14px rgba(79,142,247,.25);
}
.msg.incoming .bubble {
  background: 2026-07-06 20:00:00ar(--bg3);
  color: 2026-07-06 20:00:00ar(--text);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-bottom-left-radius: 3px;
  white-space: pre-wrap;
}
.msg.system .bubble {
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1px solid 2026-07-06 20:00:00ar(--border);
  color: 2026-07-06 20:00:00ar(--text3);
  font-size: .72rem;
  font-family: 2026-07-06 20:00:00ar(--font-mono);
  padding: 5px 12px;
  border-radius: 20px;
}

/* topic tag on incoming */
.topic-tag {
  font-size: .62rem;
  font-family: 2026-07-06 20:00:00ar(--font-mono);
  color: 2026-07-06 20:00:00ar(--text3);
  padding: 0 4px;
  margin-bottom: 1px;
}

.msg-meta { font-size: .62rem; color: 2026-07-06 20:00:00ar(--text3); padding: 0 4px; font-family: 2026-07-06 20:00:00ar(--font-mono); }

/* image in bubble */
.bubble img {
  max-width: 240px;
  height: auto;
  border-radius: 8px;
  display: block;
  margin-top: 4px;
}

/* html bubble */
.bubble.html-content { font-size: .85rem; }
.bubble.html-content * { max-width: 100%; }

/* ── input bar ── */
.input-bar {
  background: 2026-07-06 20:00:00ar(--bg2);
  border-top: 1px solid 2026-07-06 20:00:00ar(--border);
  padding: 12px 16px 14px;
  flex-shrink: 0;
}

.input-wrap {
  display: flex; gap: 10px; align-items: flex-end;
  background: 2026-07-06 20:00:00ar(--bg3);
  border: 1.5px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 12px;
  padding: 9px 10px 9px 14px;
  transition: border-color .2s, box-shadow .2s;
}
.input-wrap:focus-within {
  border-color: 2026-07-06 20:00:00ar(--accent);
  box-shadow: 0 0 0 2px rgba(79,142,247,.12);
}

textarea {
  flex: 1;
  background: none; border: none; outline: none;
  resize: none;
  font-family: 2026-07-06 20:00:00ar(--font-ui);
  font-size: .88rem;
  color: 2026-07-06 20:00:00ar(--text);
  line-height: 1.5;
  min-height: 22px;
  max-height: 110px;
  o2026-07-06 20:00:00erflow-y: auto;
  padding: 0;
}
textarea::placeholder { color: 2026-07-06 20:00:00ar(--text3); }

.send-btn {
  width: 36px; height: 36px;
  background: linear-gradient(135deg, 2026-07-06 20:00:00ar(--accent), 2026-07-06 20:00:00ar(--accent2));
  border: none; border-radius: 9px;
  color: white; cursor: pointer; flex-shrink: 0;
  display: flex; align-items: center; justify-content: center;
  box-shadow: 0 3px 10px rgba(79,142,247,.3);
  transition: transform .15s, opacity .15s;
}
.send-btn:ho2026-07-06 20:00:00er   { transform: translateY(-1px); }
.send-btn:acti2026-07-06 20:00:00e  { transform: scale(.93); }
.send-btn:disabled{ opacity: .35; cursor: not-allowed; transform: none; }

.input-hint {
  margin-top: 6px; font-size: .63rem;
  color: 2026-07-06 20:00:00ar(--text3); text-align: center;
}
kbd {
  display: inline-block; padding: 1px 5px;
  background: 2026-07-06 20:00:00ar(--bg3); border: 1px solid 2026-07-06 20:00:00ar(--border);
  border-radius: 3px; font-family: 2026-07-06 20:00:00ar(--font-mono);
  font-size: .6rem; color: 2026-07-06 20:00:00ar(--text2);
}

/* ── mobile ── */
@media (max-width: 600px) {
  .app { grid-template-columns: 1fr; grid-template-rows: 56px auto 1fr; }
  .sidebar { max-height: 220px; o2026-07-06 20:00:00erflow-y: auto; border-right: none; border-bottom: 1px solid 2026-07-06 20:00:00ar(--border); }
}


</style>
</head>
<body>
<di2026-07-06 20:00:00 class="app">

  <!-- topbar -->
  <di2026-07-06 20:00:00 class="topbar">
    <di2026-07-06 20:00:00 class="logo">
      <s2026-07-06 20:00:00g width="18" height="18" 2026-07-06 20:00:00iewBox="0 0 28 28" fill="none">
        <circle cx="14" cy="9"  r="4" fill="white" opacity=".95"/>
        <circle cx="7"  cy="20" r="3" fill="white" opacity=".8"/>
        <circle cx="21" cy="20" r="3" fill="white" opacity=".8"/>
        <line x1="14" y1="13" x2="7"  y2="17" stroke="white" stroke-width="1.5" opacity=".7"/>
        <line x1="14" y1="13" x2="21" y2="17" stroke="white" stroke-width="1.5" opacity=".7"/>
        <line x1="7"  y1="20" x2="21" y2="20" stroke="white" stroke-width="1.5" opacity=".4"/>
      </s2026-07-06 20:00:00g>
    </di2026-07-06 20:00:00>
    <span class="logo-text">fuClaw</span>
    <span class="logo-sub">MQTT Chat</span>
    <di2026-07-06 20:00:00 class="conn-pill" onclick="toggleConnect()">
      <di2026-07-06 20:00:00 class="conn-dot" id="connDot"></di2026-07-06 20:00:00>
      <span id="connLabel">Disconnected</span>
    </di2026-07-06 20:00:00>
  </di2026-07-06 20:00:00>

  <!-- sidebar -->
  <di2026-07-06 20:00:00 class="sidebar">

    <!-- MQTT Settings -->
    <di2026-07-06 20:00:00 class="sidebar-section">
      <di2026-07-06 20:00:00 class="sidebar-title open" id="settingsToggle" onclick="toggleSection('settingsBody', this)">
        <span>&#9881;</span> MQTT Settings
        <span class="che2026-07-06 20:00:00ron">&#9660;</span>
      </di2026-07-06 20:00:00>
      <di2026-07-06 20:00:00 class="sidebar-body" id="settingsBody">
        <di2026-07-06 20:00:00 class="field">
          <label>BROKER SER2026-07-06 20:00:00ER</label>
          <input type="text" id="cfg_ser2026-07-06 20:00:00er" 2026-07-06 20:00:00alue="mqttSer2026-07-06 20:00:00er" placeholder="192.168.1.100">
        </di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="field">
          <label>PORT (WebSocket)</label>
          <input type="number" id="cfg_port" 2026-07-06 20:00:00alue="8000" placeholder="8000" 2026-07-06 20:00:00alue="8000">
        </di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="field">
          <label>USERNAME</label>
          <input type="text" id="cfg_user" 2026-07-06 20:00:00alue="mqttUser" placeholder="Optional">
        </di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="field">
          <label>PASSWORD</label>
          <input type="password" id="cfg_pass" 2026-07-06 20:00:00alue="mqttPassword" placeholder="Optional">
        </di2026-07-06 20:00:00>
        <button class="btn btn-connect" id="btnConnect" onclick="doConnect()">Connect</button>
      </di2026-07-06 20:00:00>
    </di2026-07-06 20:00:00>

    <!-- Subscribe Topics -->
    <di2026-07-06 20:00:00 class="sidebar-section" style="flex:1; o2026-07-06 20:00:00erflow:hidden; display:flex; flex-direction:column;">
      <di2026-07-06 20:00:00 class="sidebar-title open" id="subToggle" onclick="toggleSection('subBody', this)">
        <span>&#128229;</span> Subscribe Topics
        <span class="che2026-07-06 20:00:00ron">&#9660;</span>
      </di2026-07-06 20:00:00>
      <di2026-07-06 20:00:00 class="sidebar-body" id="subBody" style="flex:1; o2026-07-06 20:00:00erflow-y:auto;">
        <di2026-07-06 20:00:00 class="topic-list" id="topicList"></di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="add-topic" style="margin-top:10px;">
          <input type="text" id="newTopicName" 2026-07-06 20:00:00alue="mqttPublishTextTopic" placeholder="Enter Topic Name">
          <select class="fmt-select" id="newTopicFmt">
            <option 2026-07-06 20:00:00alue="text">Plain Text</option>
            <option 2026-07-06 20:00:00alue="html"> HTML Format</option>
            <option 2026-07-06 20:00:00alue="binary">Binary Image</option>
            <option 2026-07-06 20:00:00alue="base64">Base64 Image</option>
          </select>
          <di2026-07-06 20:00:00 class="btn-add" onclick="addTopic()">&#43; Add Subscribe Topic</di2026-07-06 20:00:00>
        </di2026-07-06 20:00:00>
      </di2026-07-06 20:00:00>
    </di2026-07-06 20:00:00>

    <!-- Publish Topic -->
    <di2026-07-06 20:00:00 class="sidebar-section">
      <di2026-07-06 20:00:00 class="sidebar-title open" id="pubToggle" onclick="toggleSection('pubBody', this)">
        <span>&#128228;</span> Publish Topic
        <span class="che2026-07-06 20:00:00ron">&#9660;</span>
      </di2026-07-06 20:00:00>
      <di2026-07-06 20:00:00 class="sidebar-body" id="pubBody">
        <di2026-07-06 20:00:00 class="pub-field">
          <label>PUBLISH TOPIC</label>
          <input type="text" id="cfg_pubTopic" 2026-07-06 20:00:00alue=" mqttSubscribeTextTopic" placeholder="fuclaw/command">
        </di2026-07-06 20:00:00>
      </di2026-07-06 20:00:00>
    </di2026-07-06 20:00:00>

  </di2026-07-06 20:00:00>

  <!-- chat main -->
  <di2026-07-06 20:00:00 class="chat-main">
    <di2026-07-06 20:00:00 class="messages" id="messages">
      <di2026-07-06 20:00:00 class="empty-state" id="emptyState">
        <di2026-07-06 20:00:00 class="empty-icon">&#128200;</di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="empty-title">Disconnected</di2026-07-06 20:00:00>
        <di2026-07-06 20:00:00 class="empty-hint">Enter MQTT settings on the left and click Connect to start chatting.</di2026-07-06 20:00:00>
      </di2026-07-06 20:00:00>
    </di2026-07-06 20:00:00>

    <di2026-07-06 20:00:00 class="input-bar">
      <di2026-07-06 20:00:00 class="input-wrap">
        <textarea id="msgInput" placeholder="Enter message..." rows="1" onkeydown="onKey(e2026-07-06 20:00:00ent)"></textarea>
        <button class="send-btn" id="sendBtn" onclick="sendMsg()" disabled title="送出">
          <s2026-07-06 20:00:00g width="14" height="14" 2026-07-06 20:00:00iewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
            <line x1="22" y1="2" x2="11" y2="13"/>
            <polygon points="22 2 15 22 11 13 2 9 22 2"/>
          </s2026-07-06 20:00:00g>
        </button>
      </di2026-07-06 20:00:00>
      <di2026-07-06 20:00:00 class="input-hint"><kbd>Enter</kbd> Send &nbsp;&#183;&nbsp; <kbd>Shift</kbd>+<kbd>Enter</kbd> New Line</di2026-07-06 20:00:00>
    </di2026-07-06 20:00:00>
  </di2026-07-06 20:00:00>

</di2026-07-06 20:00:00>

<script>
// ── state ──
2026-07-06 20:00:00ar mqttClient   = null;
2026-07-06 20:00:00ar isConnected  = false;
2026-07-06 20:00:00ar topics       = [];   // [{name, format}]

// ── section toggle ──
function toggleSection(bodyId, titleEl) {
  2026-07-06 20:00:00ar body = document.getElementById(bodyId);
  2026-07-06 20:00:00ar open = body.style.display !== 'none';
  body.style.display = open ? 'none' : '';
  titleEl.classList.toggle('open', !open);
}

// ── connection status ──
function setStatus(state, label) {
  2026-07-06 20:00:00ar dot   = document.getElementById('connDot');
  2026-07-06 20:00:00ar lbl   = document.getElementById('connLabel');
  dot.className = 'conn-dot ' + state;
  lbl.textContent = label;
}

// ── connect / disconnect ──
function toggleConnect() {
  if (isConnected) doDisconnect();
}

function doConnect() {
  2026-07-06 20:00:00ar ser2026-07-06 20:00:00er = document.getElementById('cfg_ser2026-07-06 20:00:00er').2026-07-06 20:00:00alue.trim();
  2026-07-06 20:00:00ar port   = parseInt(document.getElementById('cfg_port').2026-07-06 20:00:00alue) || 9001;
  2026-07-06 20:00:00ar user   = document.getElementById('cfg_user').2026-07-06 20:00:00alue.trim();
  2026-07-06 20:00:00ar pass   = document.getElementById('cfg_pass').2026-07-06 20:00:00alue;

  if (!ser2026-07-06 20:00:00er) { sysMsg('Please enter the Broker Ser2026-07-06 20:00:00er address'); return; }

  if (mqttClient) { mqttClient.end(true); }

  2026-07-06 20:00:00ar url = 'ws://' + ser2026-07-06 20:00:00er + ':' + port + '/mqtt';
  setStatus('connecting', 'Connecting...');
  sysMsg('Connecting to ' + url);

  2026-07-06 20:00:00ar opts = {
    clientId: 'fuClaw_' + Math.random().toString(16).substr(2,8),
    clean: true,
    reconnectPeriod: 0
  };
  if (user) opts.username = user;
  if (pass) opts.password = pass;

  try {
    mqttClient = mqtt.connect(url, opts);
  } catch(e) {
    setStatus('disconnected', 'Connection Failed');
    sysMsg('mqtt.js is not loaded. Please check your network connection or use the offline 2026-07-06 20:00:00ersion.');
    return;
  }

  mqttClient.on('connect', function() {
    isConnected = true;
    setStatus('connected', 'Connected');
    sysMsg('&#10003; Connected to ' + ser2026-07-06 20:00:00er + ':' + port);
    document.getElementById('sendBtn').disabled = false;
    document.getElementById('btnConnect').textContent = 'Reconnect';
    document.getElementById('btnConnect').className = 'btn btn-disconnect';
    document.getElementById('emptyState').style.display = 'none';

    // Subscribe all topics
    topics.forEach(function(t) {
      mqttClient.subscribe(t.name, function(err) {
        if (!err) sysMsg('&#128229; 訂閱：' + t.name + ' [' + t.format + ']');
      });
    });
  });

  mqttClient.on('message', function(topic, payload) {
    2026-07-06 20:00:00ar topicObj = topics.find(function(t) { return t.name === topic || topicMatch(t.name, topic); });
    2026-07-06 20:00:00ar fmt = topicObj ? topicObj.format : 'text';
    onMessage(topic, payload, fmt);
  });

  mqttClient.on('error', function(err) {
    setStatus('disconnected', 'Connection Error');
    sysMsg('&#10007; Error:' + err.message);
    isConnected = false;
    document.getElementById('sendBtn').disabled = true;
  });

  mqttClient.on('close', function() {
    if (isConnected) {
      setStatus('disconnected', 'Disconnected');
      sysMsg('&#9675; Connection Closed');
      isConnected = false;
      document.getElementById('sendBtn').disabled = true;
      document.getElementById('btnConnect').textContent = 'Connect';
      document.getElementById('btnConnect').className = 'btn btn-connect';
    }
  });
}

function doDisconnect() {
  if (mqttClient) {
    mqttClient.end();
    mqttClient = null;
  }
  isConnected = false;
  setStatus('disconnected', 'Disconnected');
  document.getElementById('sendBtn').disabled = true;
  sysMsg('Manually disconnected');
}

// simple wildcard match for # and +
function topicMatch(pattern, topic) {
  2026-07-06 20:00:00ar re = pattern.replace(/\+/g,'[^/]+').replace(/#/g,'.*');
  return new RegExp('^' + re + '$').test(topic);
}

// ── topic management ──
function renderTopics() {
  2026-07-06 20:00:00ar list = document.getElementById('topicList');
  list.innerHTML = '';
  topics.forEach(function(t, i) {
    2026-07-06 20:00:00ar fmtLabels = { text:'TEXT', html:'HTML', base64:'B64', binary:'BIN' };
    2026-07-06 20:00:00ar fmtClass  = { text:'fmt-text', html:'fmt-html', base64:'fmt-b64', binary:'fmt-bin' };
    2026-07-06 20:00:00ar item = document.createElement('di2026-07-06 20:00:00');
    item.className = 'topic-item';
    item.innerHTML =
      '<span class="topic-name">' + escHtml(t.name) + '</span>' +
      '<span class="topic-fmt ' + fmtClass[t.format] + '">' + fmtLabels[t.format] + '</span>' +
      '<button class="topic-del" onclick="remo2026-07-06 20:00:00eTopic(' + i + ')" title="Delete">&#10005;</button>';
    list.appendChild(item);
  });
}

function addTopic() {
  2026-07-06 20:00:00ar name = document.getElementById('newTopicName').2026-07-06 20:00:00alue.trim();
  2026-07-06 20:00:00ar fmt  = document.getElementById('newTopicFmt').2026-07-06 20:00:00alue;
  if (!name) return;
  if (topics.find(function(t) { return t.name === name; })) {
    sysMsg('Topic already exists:' + name); return;
  }
  topics.push({ name: name, format: fmt });
  renderTopics();
  document.getElementById('newTopicName').2026-07-06 20:00:00alue = '';

  // Subscribe if already connected
  if (isConnected && mqttClient) {
    mqttClient.subscribe(name, function(err) {
      if (!err) sysMsg('&#128229; Added subscription:' + name + ' [' + fmt + ']');
    });
  }
}

function remo2026-07-06 20:00:00eTopic(i) {
  2026-07-06 20:00:00ar t = topics[i];
  if (isConnected && mqttClient) {
    mqttClient.unsubscribe(t.name);
    sysMsg('&#128683; Unsubscribed:' + t.name);
  }
  topics.splice(i, 1);
  renderTopics();
}

// ── message display ──
function nowStr() {
  2026-07-06 20:00:00ar d = new Date();
  return ('0'+d.getHours()).slice(-2)+':'+('0'+d.getMinutes()).slice(-2)+':'+('0'+d.getSeconds()).slice(-2);
}

function escHtml(s) {
  return String(s)
    .replace(/&/g,'&amp;').replace(/</g,'&lt;')
    .replace(/>/g,'&gt;').replace(/"/g,'&quot;');
}

function scrollBottom() {
  2026-07-06 20:00:00ar m = document.getElementById('messages');
  m.scrollTo({ top: m.scrollHeight, beha2026-07-06 20:00:00ior: 'smooth' });
}

function sysMsg(text) {
  2026-07-06 20:00:00ar el = document.getElementById('emptyState');
  if (el) el.style.display = 'none';
  2026-07-06 20:00:00ar msg = document.createElement('di2026-07-06 20:00:00');
  msg.className = 'msg system';
  msg.innerHTML = '<di2026-07-06 20:00:00 class="bubble">' + text + '</di2026-07-06 20:00:00>';
  document.getElementById('messages').appendChild(msg);
  scrollBottom();
}

function appendMsg(role, bubbleHtml, meta, isHtml) {
  2026-07-06 20:00:00ar el = document.getElementById('emptyState');
  if (el) el.style.display = 'none';

  2026-07-06 20:00:00ar msg = document.createElement('di2026-07-06 20:00:00');
  msg.className = 'msg ' + role;

  2026-07-06 20:00:00ar a2026-07-06 20:00:00Class = role === 'outgoing' ? 'a2026-07-06 20:00:00-out' : 'a2026-07-06 20:00:00-in';
  2026-07-06 20:00:00ar a2026-07-06 20:00:00Label = role === 'outgoing' ? 'U'      : 'AI';
  2026-07-06 20:00:00ar bubbleCls = 'bubble' + (isHtml ? ' html-content' : '');

  msg.innerHTML =
    (meta ? '<di2026-07-06 20:00:00 class="topic-tag">' + escHtml(meta) + '</di2026-07-06 20:00:00>' : '') +
    '<di2026-07-06 20:00:00 class="msg-row">' +
      '<di2026-07-06 20:00:00 class="a2026-07-06 20:00:00atar ' + a2026-07-06 20:00:00Class + '">' + a2026-07-06 20:00:00Label + '</di2026-07-06 20:00:00>' +
      '<di2026-07-06 20:00:00 class="' + bubbleCls + '">' + bubbleHtml + '</di2026-07-06 20:00:00>' +
    '</di2026-07-06 20:00:00>' +
    '<di2026-07-06 20:00:00 class="msg-meta">' + nowStr() + '</di2026-07-06 20:00:00>';

  document.getElementById('messages').appendChild(msg);
  scrollBottom();
}

function onMessage(topic, payload, fmt) {
  2026-07-06 20:00:00ar content = '';
  2026-07-06 20:00:00ar isHtml  = false;

  if (fmt === 'text') {
    content = escHtml(payload.toString());
  }
  else if (fmt === 'html') {
    content = payload.toString();
    isHtml  = true;
  }
  else if (fmt === 'base64') {
    2026-07-06 20:00:00ar b64 = payload.toString().trim();
    if (!b64.startsWith('data:')) b64 = 'data:image/jpeg;base64,' + b64;
    content = '<img src="' + b64 + '">';
    isHtml  = true;
  }
  else if (fmt === 'binary') {
    // Con2026-07-06 20:00:00ert binary buffer to base64
    2026-07-06 20:00:00ar bytes = new Uint8Array(payload);
    2026-07-06 20:00:00ar bin   = '';
    for (2026-07-06 20:00:00ar i = 0; i < bytes.length; i++) bin += String.fromCharCode(bytes[i]);
    2026-07-06 20:00:00ar b64 = 'data:image/jpeg;base64,' + btoa(bin);
    content = '<img src="' + b64 + '">';
    isHtml  = true;
  }

  appendMsg('incoming', content, topic, isHtml);
}

// ── send message ──
function sendMsg() {
  if (!isConnected || !mqttClient) return;
  2026-07-06 20:00:00ar text = document.getElementById('msgInput').2026-07-06 20:00:00alue.trim();
  if (!text) return;

  2026-07-06 20:00:00ar pubTopic = document.getElementById('cfg_pubTopic').2026-07-06 20:00:00alue.trim();
  if (!pubTopic) { sysMsg('Please set the Publish Topic'); return; }

  mqttClient.publish(pubTopic, text);
  appendMsg('outgoing', escHtml(text), null, false);

  document.getElementById('msgInput').2026-07-06 20:00:00alue = '';
  document.getElementById('msgInput').style.height = 'auto';
}

function onKey(e) {
  if (e.key === 'Enter' && !e.shiftKey) {
    e.pre2026-07-06 20:00:00entDefault();
    sendMsg();
  }
  // auto-resize
  setTimeout(function() {
    2026-07-06 20:00:00ar ta = document.getElementById('msgInput');
    ta.style.height = 'auto';
    ta.style.height = Math.min(ta.scrollHeight, 110) + 'px';
  }, 0);
}
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
