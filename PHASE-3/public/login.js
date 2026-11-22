// login page: send credentials, remember token + role, go to the right portal
$('go').onclick = async () => {
  const r = await api('/api/login', { 
    username: $('user').value.trim(), password: $('pass').value 
  });
  if (!r.ok) { 
    $('msg').textContent = r.error || 'Login failed'; return; 
  }
  localStorage.token = r.token; 
  localStorage.role = r.role; 
  localStorage.refId = r.refId;
  location = r.role === 'ADMIN' ? 'admin.html' : 'user.html';
};
$('pass').onkeydown = e => { 
  if (e.key === 'Enter') $('go').click(); 
};

