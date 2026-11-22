// helpers shared by all pages
const $ = id => document.getElementById(id);
const esc = s => String(s).replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));

// GET when no body is given, POST (or given method) otherwise
async function api(url, body, method) {
  const opt = { headers: { 'Content-Type': 'application/json', 'x-token': localStorage.token || '' } };
  if (body !== undefined) { opt.method = method || 'POST'; opt.body = typeof body === 'string' ? body : JSON.stringify(body); }
  const r = await fetch(url, opt);
  if (r.status === 401 && !url.includes('login')) logout();
  return r.json();
}
function guard(role) { if (localStorage.role !== role) location = '/'; }
function logout() { localStorage.clear(); location = '/'; }

// cols = [['Header', 'key'], ...]
function table(rows, cols) {
  if (!rows.length) return '<p>No records.</p>';
  const head = cols.map(c => `<th>${c[0]}</th>`).join('');
  const body = rows.map(r => '<tr>' + cols.map(c => `<td>${esc(r[c[1]] ?? '')}</td>`).join('') + '</tr>').join('');
  return `<div class="scroll"><table><tr>${head}</tr>${body}</table></div>`;
}
function fill(id, items, first) {
  $(id).innerHTML = (first ? `<option>${first}</option>` : '') + items.map(i => `<option>${esc(i)}</option>`).join('');
}
function tabs(ids) {   // show one section at a time
  document.querySelectorAll('nav button').forEach(b => b.onclick = () => {
    ids.forEach(i => $(i).classList.toggle('on', i === b.dataset.tab));
  });
  $(ids[0]).classList.add('on');
}

