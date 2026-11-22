// student page: own seat for every exam date | faculty page: own duties + UFM reports
const role = localStorage.role;
if (!role) location = '/';
$('title').textContent = `ExamSync - ${role} ${localStorage.refId}`;

async function student() {
  $('student').classList.add('on');
  const r = await api('/api/my/seat');
  $('seatOut').innerHTML = (r.found ? `<p><b>${esc(r.name)}</b> | ${esc(r.course)} ${esc(r.branch)} | Sem ${r.sem}</p>` : '') +
    (r.seats && r.seats.length ? table(r.seats, [['Date', 'date'], ['Building', 'building'], ['Floor', 'floor'], ['Room', 'room'], ['Row', 'row'], ['Col', 'col']])
      : '<p>Seating not allotted yet.</p>');
}

async function faculty() {
  $('faculty').classList.add('on');
  $('dutyOut').innerHTML = table(await api('/api/my/duties'), [['Date', 'date'], ['Time', 'time'], ['Building', 'building'], ['Room', 'room'], ['Role', 'role']]);
  $('ufmOut').innerHTML = table(await api('/api/my/ufm'), [['ID', 'id'], ['Roll', 'roll'], ['Subject', 'subject'], ['Room', 'room'], ['Type', 'type'], ['Status', 'status']]);
}
$('add').onclick = async () => {
  await api('/api/my/ufm', { roll: $('roll').value, subject: $('subj').value, room: $('room').value, type: $('itype').value, details: $('details').value });
  $('msg').innerHTML = '<span class="ok">Report submitted.</span>';
  faculty();
};

if (role === 'STUDENT') student(); else faculty();

