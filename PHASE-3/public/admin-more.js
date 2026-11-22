// Admin: student search, duty scheduling, UFM reports, file upload

// student search: C++ uses binary search (O(log n)) + hash index for seats
$('stuBtn').onclick = async () => {
  const r = await api('/api/admin/student/' + encodeURIComponent($('roll').value.trim()));
  if (!r.found) { $('stuOut').innerHTML = '<p class="msg">Student not found.</p>'; return; }
  $('stuOut').innerHTML = `<p><b>${esc(r.name)}</b> | ${esc(r.roll)} | ${esc(r.course)} ${esc(r.branch)} | Sem ${r.sem}
    <br>Binary search found it in ${r.steps} steps</p>` +
    table(r.seats, [['Date', 'date'], ['Shift', 'shift'], ['Room', 'room'], ['Building', 'building'], ['Floor', 'floor'], ['Row', 'row'], ['Col', 'col'], ['Paper', 'code']]);
};

// duty scheduling uses a min-heap so every teacher gets a fair number of duties
$('dutyBtn').onclick = async () => {
  $('dutyMsg').textContent = 'Scheduling...';
  const r = await api('/api/admin/duty', {});
  $('dutyMsg').innerHTML = `<span class="ok">${r.duties} duties assigned to ${r.faculty} faculty.</span>`;
};
$('dutyView').onclick = async () => {
  const r = await api(`/api/admin/duties?fid=${encodeURIComponent($('fid').value.trim() || '-')}&date=${$('dutyDate').value}`);
  $('dutyOut').innerHTML = table(r, [['Faculty ID', 'fid'], ['Name', 'name'], ['Date', 'date'], ['Time', 'time'], ['Building', 'building'], ['Room', 'room'], ['Role', 'role']]);
};

// UFM (unfair means) reports
const ufmCols = [['ID', 'id'], ['Roll', 'roll'], ['Subject', 'subject'], ['Room', 'room'], ['Faculty', 'faculty'], ['Time', 'time'], ['Type', 'type'], ['Details', 'details'], ['Status', 'status']];
const showUfm = async q => $('ufmOut').innerHTML = table(await api('/api/admin/ufm' + q), ufmCols);
$('ufmAll').onclick = () => showUfm('');
$('ufmPend').onclick = () => showUfm('?pending=1');
$('ufmSet').onclick = async () => {
  await api('/api/admin/ufm/' + encodeURIComponent($('uid').value.trim()), { status: $('ustatus').value });
  showUfm('');
};

// upload csv: replaces students / exam schedule / rooms file on the server
document.querySelectorAll('[data-k]').forEach(b => b.onclick = async () => {
  const f = $('f_' + b.dataset.k).files[0];
  if (!f) { $('upMsg').textContent = 'Choose a file first.'; return; }
  const r = await (await fetch('/api/admin/upload/' + b.dataset.k, {
    method: 'POST', headers: { 'Content-Type': 'text/plain', 'x-token': localStorage.token }, body: await f.text() })).json();
  $('upMsg').innerHTML = r.ok ? `<span class="ok">Uploaded ${r.lines} rows.</span>` : `<span class="msg">${esc(r.error)}</span>`;
  loadDates();
});

