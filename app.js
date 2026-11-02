
(() => {
'use strict';
const $=id=>document.getElementById(id), KEY='esports-arena-v3', SIMS=10000;
let W=null, mode='real', editId=-1, lastChampion=-1, odds={}, activePlayer=+(localStorage.getItem('arena-player')||0);
const call=(name,...args)=>W.ccall(name, name==='api_probability'?'number':'number', args.map(x=>typeof x==='string'?'string':typeof x==='number'?'number':'number'),args);
const str=name=>id=>W.ccall(name,'string',['number'],[id]);
const teamName=str('api_team_name'),teamTag=str('api_team_tag'),teamColor=str('api_team_color');
const say=t=>$('notice').textContent=t;
function persist(){if(!W)return;try{localStorage.setItem(KEY,W.ccall('api_export','string',[],[]));}catch(e){say('Local save failed: '+e.message)}}
function history(){try{return JSON.parse(localStorage.getItem('arena-odds-history')||'[]')}catch(_){return []}}
function recordHistory(){const s=state();if(!s||s.status<2)return;const vals=getOdds(s),h=history();h.push({at:new Date().toISOString(),odds:vals});localStorage.setItem('arena-odds-history',JSON.stringify(h.slice(-24)))}
function state(){try{return JSON.parse(W.ccall('api_state','string',[],[]))}catch(e){return null}}
function activeTeams(){const s=state();return s?s.teams:[]}
function error(){say(W.ccall('api_error','string',[],[])||'The C engine rejected that operation.')}
function callOK(name,...args){const n=W.ccall(name,'number',args.map(a=>typeof a==='string'?'string':'number'),args);if(!n)error();else{persist();render()}return !!n}
function initials(n){return n.split(/\s+/).map(x=>x[0]||'').join('').slice(0,3).toUpperCase()}
function crest(id){const t=activeTeams().find(x=>x.id===id);return `<i class="crest" style="background:${t?.color||'#48e8ff'}">${initials(t?.name||'?')}</i>`}
function render(){
 const s=state();if(!s)return;
 $('create-panel').hidden=s.status!==0;
 $('registration').hidden=s.status!==1;
 $('live-panel').hidden=s.status!==2&&s.status!==3;
 $('team-count').textContent=`${s.teams.length} / ${s.size} TEAMS`;
 $('roster').innerHTML=s.teams.map(t=>`<div class="roster-card"><i class="crest" style="background:${t.color}">${initials(t.name)}</i><span><b>${esc(t.name)}</b><small>${esc(t.tag||'Roster member')} · ${t.rating} ELO</small></span><button data-edit="${t.id}" title="Edit team">EDIT</button><button data-remove="${t.id}" title="Remove team">×</button></div>`).join('');
 $('start').disabled=s.teams.length!==s.size;
 if(s.status>=2){
  $('event-title').textContent=s.name;$('event-sub').textContent=`${s.game} · ${s.size}-team single elimination · C11 engine`;
  $('stats').innerHTML=stat('REGISTERED',s.teams.length+'/'+s.size)+stat('MATCHES PLAYED',s.winners.filter(x=>x>=0).length+'/'+(s.size-1))+stat('ROUNDS',Math.log2(s.size))+stat('STATUS',s.status===3?'CHAMPION':'LIVE');
  renderBracket(s);renderOdds(s);renderLeaderboard(s);renderAnalytics(s);
 }
}
function esc(s){return String(s).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]))}
function stat(a,b){return `<div class="stat"><small>${a}</small><strong>${b}</strong></div>`}
const rounds=['ROUND OF 16','QUARTERFINALS','SEMIFINALS','GRAND FINAL'];
function roundName(r,n){return n===4?['SEMIFINALS','GRAND FINAL'][r]:n===3?['QUARTERFINALS','SEMIFINALS','GRAND FINAL'][r]:rounds[r]}
function renderBracket(s){
 const n=Math.log2(s.size),parts=[];
 for(let r=0;r<n;r++){
  let html=`<div class="round"><h3>${roundName(r,n)}</h3><div class="round-matches">`;
  const count=s.size>>(r+1);
  for(let m=0;m<count;m++){
   const a=W.ccall(mode==='picks'?'api_pick_team':mode==='upset'?'api_upset_team':mode==='predict'?'api_prediction_team':'api_match_team','number',mode==='picks'?['number','number','number','number']:['number','number','number'],mode==='picks'?[activePlayer,r,m,0]:[r,m,0]),b=W.ccall(mode==='picks'?'api_pick_team':mode==='upset'?'api_upset_team':mode==='predict'?'api_prediction_team':'api_match_team','number',mode==='picks'?['number','number','number','number']:['number','number','number'],mode==='picks'?[activePlayer,r,m,1]:[r,m,1]);
   const actual=W.ccall('api_match_winner','number',['number','number'],[r,m]);
   const w=(mode==='predict'||mode==='upset')?-1:actual;
   const predicted=mode==='picks'?W.ccall('api_player_pick','number',['number','number','number'],[activePlayer,r,m]):mode==='predict'?W.ccall('api_prediction_winner','number',['number','number'],[r,m]):mode==='upset'?W.ccall('api_upset_winner','number',['number','number'],[r,m]):-1;
   html+=`<div class="match ${r===n-1?'final':''}">${teamRow(a,b,r,m,w,predicted)}${teamRow(b,a,r,m,w,predicted)}</div>`;
  }
  html+='</div></div>';parts.push(html);
 }
 const champion=W.ccall('api_champion','number',[],[]);
 parts.push(`<div class="round"><h3>CHAMPION</h3><div class="round-matches"><div class="match final">${champion>=0?`<div class="team-row winner">${crest(champion)}<strong>${esc(teamName(champion))}</strong> ✦</div>`:'<div class="team-row pending">Awaiting champion</div>'}</div></div></div>`);
 $('bracket').innerHTML=parts.join('');
 if(champion>=0&&champion!==lastChampion){lastChampion=champion;$('champion-name').textContent=teamName(champion);$('champion-modal').hidden=false;confetti()}
}
function teamRow(id,op,r,m,w,pred){
 if(id<0)return '<div class="team-row pending">Awaiting qualification</div>';
 const color=teamColor(id),name=esc(teamName(id)),prob=op>=0?Math.round(100*W.ccall('api_probability','number',['number','number'],[id,op]))+'%':'';
 const cls=w>=0?(id===w?'winner':'loser'):pred===id?'predicted':'';
 return `<div class="team-row ${cls}" style="--team-color:${color}" data-team="${id}" data-round="${r}" data-match="${m}">${crest(id)}<span>${name}</span><b>${w>=0?(id===w?'WIN':'—'):prob}</b></div>`;
}
function getOdds(s){
 const ptr=W._malloc(16*8);W.ccall('api_simulate','number',['number','number','number'],[SIMS,0,ptr]);
 const arr=Array.from(W.HEAPF64.slice(ptr/8,ptr/8+16));W._free(ptr);return arr;
}
function teamRecord(id){return `${W.ccall('api_team_wins','number',['number'],[id])}W / ${W.ccall('api_team_losses','number',['number'],[id])}L`}
function renderOdds(s){
 odds=getOdds(s);const rows=s.teams.slice().sort((a,b)=>odds[b.id]-odds[a.id]);
 $('odds').innerHTML=rows.map(t=>`<div class="odds-row"><div class="odds-top">${crest(t.id)}<span>${esc(t.name)}</span><b>${odds[t.id].toFixed(1)}%</b></div><div class="meter" style="--team-color:${t.color}"><i style="width:${odds[t.id]}%;--team-color:${t.color}"></i></div><div class="rating-control"><small>ELO ${t.rating} · ${teamRecord(t.id)}</small><input aria-label="Set ${esc(t.name)} rating" type="range" min="500" max="2500" step="10" value="${t.rating}" data-rating="${t.id}"></div></div>`).join('');
 const h=history();if(h.length>1){const Wd=300,Ht=100;const lines=s.teams.map(t=>{const pts=h.map((v,i)=>`${10+i*280/(h.length-1)},${90-(v.odds[t.id]||0)*.8}`).join(' ');return `<polyline fill="none" stroke="${t.color}" stroke-width="2" points="${pts}"/>`}).join('');$('odds-history').innerHTML=`<div class="kicker">ODDS HISTORY</div><svg viewBox="0 0 ${Wd} ${Ht}" role="img" aria-label="Team odds history">${lines}</svg><small class="muted">${h.length} snapshots · recorded results</small>`}else $('odds-history').innerHTML='<p class="muted">Odds history appears after match results change.</p>';
}
function renderLeaderboard(s){
 let count=W.ccall('api_players_count','number',[],[]);
 if(count===0){const nick=localStorage.getItem('arena-nick')||'Player';W.ccall('api_player_name','number',['number','string'],[0,nick]);count=1;}
 if(activePlayer>=count)activePlayer=0;
 $('player-name').value=W.ccall('api_player_nick','string',['number'],[activePlayer])||'Player';
 const rows=Array.from({length:count},(_,i)=>({id:i,nick:W.ccall('api_player_nick','string',['number'],[i])||`Player ${i+1}`,score:W.ccall('api_score','number',['number'],[i]),accuracy:W.ccall('api_accuracy','number',['number'],[i]),rank:W.ccall('api_rank','number',['number'],[i])})).sort((a,b)=>b.score-a.score||a.nick.localeCompare(b.nick));
 $('leaderboard').innerHTML=rows.map(r=>`<div class="board-row ${r.id===activePlayer?'active-player':''}" data-player="${r.id}"><span>${r.rank}. ${esc(r.nick)}${r.id===activePlayer?' <small>(ACTIVE)</small>':''}</span><b>${r.score} pts · ${r.accuracy.toFixed(0)}%</b></div>`).join('')+`<p class="muted">${count} local players · scores update when official results are entered.</p>`;
}
function renderAnalytics(s){
 const complete=s.winners.filter(x=>x>=0).length,progress=Math.round(100*complete/(s.size-1));
 $('analytics').innerHTML=`<div class="analytics-line"><span>Tournament progress</span><b>${progress}%</b></div><div class="analytics-line"><span>Completed matches</span><b>${complete}</b></div><div class="analytics-line"><span>Remaining matches</span><b>${s.size-1-complete}</b></div><div class="analytics-line"><span>Champion</span><b>${W.ccall('api_champion','number',[],[])>=0?esc(teamName(W.ccall('api_champion','number',[],[]))):'TBD'}</b></div>`;
}
function confetti(){if(matchMedia('(prefers-reduced-motion: reduce)').matches)return;const c=document.createElement('div');c.className='confetti';c.textContent='✦ ✧ ✦ ✧ ✦';Object.assign(c.style,{position:'fixed',inset:'15% 0',textAlign:'center',fontSize:'50px',color:'#ffda76',pointerEvents:'none',zIndex:20,textShadow:'0 0 30px #ffda76'});document.body.append(c);setTimeout(()=>c.remove(),2200)}
$('create').onclick=()=>{const ok=callOK('api_create',$('t-name').value,$('t-game').value,+$('t-size').value);if(ok){say('Tournament created. Register all teams to unlock bracket generation.');render()}};
$('register').onclick=()=>{
 const name=$('team-name').value.trim();if(!name)return say('Enter a team name.');
 const ok=callOK('api_register',editId,name,$('team-tag').value,$('team-color').value);
 if(ok){editId=-1;$('register').textContent='REGISTER TEAM +';$('team-name').value='';$('team-tag').value='';say('Team saved in the C engine.')}
};
$('roster').onclick=e=>{
 const edit=e.target.closest('[data-edit]'),del=e.target.closest('[data-remove]');
 if(edit){const id=+edit.dataset.edit,t=activeTeams().find(x=>x.id===id);if(t){editId=id;$('team-name').value=t.name;$('team-tag').value=t.tag;$('team-color').value=t.color;$('register').textContent='UPDATE TEAM ↻';$('team-name').focus()}}
 if(del){if(callOK('api_withdraw',+del.dataset.remove))say('Team withdrawn.')}
};
$('start').onclick=()=>{if(callOK('api_start'))say('Bracket generated by the C11 engine.');};
function resetEvent(){if(confirm('Reset this tournament and all local data?')){W.ccall('api_reset','void',[],[]);localStorage.removeItem(KEY);localStorage.removeItem('arena-odds-history');localStorage.removeItem('arena-player');activePlayer=0;mode='real';lastChampion=-1;render();say('Arena reset.')}}
$('reset').onclick=resetEvent;$('reset-live').onclick=resetEvent;
$('demo').onclick=()=>{
 W.ccall('api_reset','void',[],[]);W.ccall('api_create','number',['string','string','number'],['Neon Masters Invitational','Valorant',8]);
 const demo=[['NOVA','Starlight','#43eaff',1710],['VORTEX','No mercy','#ff4baf',1650],['TITAN','Built different','#9b75ff',1770],['PHOENIX','Rise again','#ff9d42',1580],['ECLIPSE','Nightfall','#56d69a',1680],['ZENITH','Above all','#ffda76',1620],['REAPERS','Fear the dark','#5d8dff',1540],['ORBIT','Beyond limits','#df65ff',1600]];
 demo.forEach((t,i)=>{W.ccall('api_register','number',['number','string','string','string'],[-1,t[0],t[1],t[2]]);W.ccall('api_rating','number',['number','number'],[i,t[3]]);});
 W.ccall('api_start','number',[],[]);
 // Sample match outcomes, deliberately leaving the final undecided.
 for(let m=0;m<4;m++){const a=W.ccall('api_match_team','number',['number','number','number'],[0,m,0]),b=W.ccall('api_match_team','number',['number','number','number'],[0,m,1]);W.ccall('api_winner','number',['number','number','number'],[0,m,(m%2)?b:a]);}
 for(let m=0;m<2;m++){const a=W.ccall('api_match_team','number',['number','number','number'],[1,m,0]),b=W.ccall('api_match_team','number',['number','number','number'],[1,m,1]);W.ccall('api_winner','number',['number','number','number'],[1,m,m===0?a:b]);}
 persist();mode='real';render();say('Demo loaded. Explore the bracket, odds and predictions.');
};
$('predict').onclick=()=>{W.ccall('api_run_prediction','number',[],[]);mode='predict';render();say('Full bracket forecast calculated by the C Elo prediction engine.');};
$('upset').onclick=()=>{W.ccall('api_upset_run','number',['number'],[0]);mode='upset';render();say('Random upset bracket generated by the C simulation engine.');};
$('clear-view').onclick=()=>{mode='real';render();say('Showing official recorded results.');};
$('my-picks').onclick=()=>{mode='picks';render();say('Pick winners for undecided matches to build your bracket.');};
$('bracket').onclick=e=>{
 const row=e.target.closest('[data-team]');if(!row)return;
 const id=+row.dataset.team,r=+row.dataset.round,m=+row.dataset.match;
 if(mode==='predict'||mode==='upset')return say('Switch to REAL RESULTS or MY BRACKET to interact with this match.');
 if(mode==='picks'){if(W.ccall('api_match_winner','number',['number','number'],[r,m])>=0)return say('This match already has an official result.');const ok=W.ccall('api_pick','number',['number','number','number','number'],[activePlayer,r,m,id]);if(ok){persist();render();say('Prediction saved in the C engine.')}else error();return}
 const current=W.ccall('api_match_winner','number',['number','number'],[r,m]);
 if(current===id)callOK('api_winner',r,m,-1);else callOK('api_winner',r,m,id);recordHistory();render();
};
$('player-name').onchange=e=>{const nick=e.target.value.trim()||'Player';if(activePlayer===0)localStorage.setItem('arena-nick',nick);W.ccall('api_player_name','number',['number','string'],[activePlayer,nick]);persist();renderLeaderboard(state())};
$('add-player').onclick=()=>{const count=W.ccall('api_players_count','number',[],[]);if(count>=32)return say('Maximum of 32 local players reached.');const nick=prompt('Enter player callsign:');if(!nick||!nick.trim())return;activePlayer=count;W.ccall('api_player_name','number',['number','string'],[activePlayer,nick.trim().slice(0,31)]);localStorage.setItem('arena-player',String(activePlayer));persist();render();say('New prediction player added.')};
$('leaderboard').onclick=e=>{const row=e.target.closest('[data-player]');if(!row)return;activePlayer=+row.dataset.player;localStorage.setItem('arena-player',String(activePlayer));render();say('Active prediction profile changed.')};
$('export').onclick=()=>{const data=JSON.parse(W.ccall('api_export','string',[],[]));data.oddsHistory=history();const blob=new Blob([JSON.stringify(data,null,2)],{type:'application/json'}),url=URL.createObjectURL(blob),a=document.createElement('a');a.href=url;a.download='esports-arena-backup.json';a.click();URL.revokeObjectURL(url);say('JSON backup exported.')};
$('import').onchange=async e=>{const f=e.target.files[0];if(!f)return;try{const data=await f.text();if(data.length>250000)throw Error('Backup is too large.');const parsed=JSON.parse(data);const ok=W.ccall('api_import','number',['string'],[JSON.stringify(parsed)]);if(!ok)throw Error('Invalid or unsupported backup. The current tournament was not changed.');if(Array.isArray(parsed.oddsHistory))localStorage.setItem('arena-odds-history',JSON.stringify(parsed.oddsHistory.slice(-24)));persist();render();say('Backup restored successfully.')}catch(err){say(err.message)}e.target.value=''};
$('close-champion').onclick=()=>$('champion-modal').hidden=true;
async function boot(){
 if(typeof createEsportsModule!=='function'){say('WASM engine not built yet. Install Emscripten and run ./scripts/build-wasm.sh.');return}
 try{
  W=await createEsportsModule();const saved=localStorage.getItem(KEY);
  if(saved){const ok=W.ccall('api_import','number',['string'],[saved]);if(!ok){localStorage.removeItem(KEY);say('Saved data was invalid; starting fresh.')}}
  else{W.ccall('api_reset','void',[],[])}
  render();say('C11 / WebAssembly engine ready. Data stays on this device.');
 }catch(e){say('Could not load WebAssembly engine: '+e.message)}
}
boot();
})();
