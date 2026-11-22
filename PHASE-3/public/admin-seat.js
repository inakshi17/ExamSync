// Admin: dates, seat allocation, rooms-by-date and room grid view
guard('ADMIN');
tabs(['allocate', 'seating', 'room', 'student', 'duty', 'ufm', 'upload']);
const T = () => $('type').value;                       // odd / even
const color = c => `hsl(${[...c].reduce((a, ch) => a * 31 + ch.charCodeAt(0), 7) % 360},70%,85%)`;
const dateCols = [['Date', 'date'], ['Students', 'students'], ['Seated', 'seated'], ['Unseated', 'unseated']];

async function loadDates() {
  const d = await api('/api/admin/dates?type=' + T());
  d.forEach(x => x.unseated = x.students - x.seated);
  const names = d.map(x => x.date);
  fill('allocDate', names, 'ALL'); fill('viewDate', names); fill('roomDate', names); fill('dutyDate', names, '-');
  $('allocOut').innerHTML = table(d, dateCols);
}
$('type').onchange = loadDates;

$('runBtn').onclick = async () => {
  $('allocMsg').textContent = 'Running allocation...';
  const r = await api('/api/admin/run', { type: T(), date: $('allocDate').value });
  r.forEach(x => x.unseated = x.students - x.seated);
  $('allocMsg').innerHTML = '<span class="ok">Done. Saved to data/seating_plan.csv</span>';
  $('allocOut').innerHTML = table(r, dateCols);
};
$('resetBtn').onclick = async () => {
  if (!confirm('Delete the whole seating plan?')) return;
  await api('/api/admin/reset', {});
  $('allocMsg').textContent = 'Seating plan cleared.';
  loadDates();
};

// rooms used on a chosen date, click a row to open that room
$('viewBtn').onclick = async () => {
  const r = await api('/api/admin/rooms?date=' + $('viewDate').value);
  $('seatOut').innerHTML = '<div class="scroll"><table><tr><th>Room</th><th>Building</th><th>Floor</th><th>Used / Capacity</th><th></th></tr>' +
    r.map(x => `<tr><td>${esc(x.room)}</td><td>${esc(x.building)}</td><td>${esc(x.floor)}</td><td>${x.used} / ${x.cap}</td>
      <td><button data-room="${esc(x.room)}">Open</button></td></tr>`).join('') + '</table></div>';
};
$('seatOut').onclick = e => {
  if (!e.target.dataset.room) return;
  $('roomQ').value = e.target.dataset.room; $('roomDate').value = $('viewDate').value;
  document.querySelector('[data-tab=room]').click(); showRoom();
};

// room grid: one coloured cell per desk, click a desk to see the student
async function showRoom() {
  const rooms = await api(`/api/admin/room?q=${encodeURIComponent($('roomQ').value)}&date=${$('roomDate').value}`);
  $('info').textContent = ''; $('roomOut').innerHTML = rooms.length ? '' : '<p>No room found.</p>';
  rooms.forEach(r => {
    const g = {}; r.seats.forEach(s => g[s.row + ',' + s.col] = s);
    let h = `<h4>${esc(r.room)} - ${esc(r.building)}, floor ${esc(r.floor)} (${r.seats.length} students)</h4><div class="scroll"><table>`;
    for (let i = 1; i <= r.rows; i++) {
      h += '<tr>';
      for (let j = 1; j <= r.cols; j++) {
        const s = g[i + ',' + j];
        h += s ? `<td class="seat" style="background:${color(s.code)}" data-i="${esc(`${s.roll} | ${s.name} | ${s.course} ${s.branch} | Sem ${s.sem}`)}">${esc(s.code)}</td>` : '<td class="seat"></td>';
      }
      h += '</tr>';
    }
    h += '</table></div>' + table(r.seats, [['Roll No', 'roll'], ['Name', 'name'], ['Course', 'course'], ['Branch', 'branch'], ['Sem', 'sem'], ['Row', 'row'], ['Col', 'col']]);
    $('roomOut').insertAdjacentHTML('beforeend', h);
  });
}
$('roomBtn').onclick = showRoom;
$('roomOut').onclick = e => { if (e.target.dataset.i) $('info').textContent = e.target.dataset.i; };
loadDates();

