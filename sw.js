const CACHE='esports-arena-v3-shell';
const FILES=['./','./index.html','./manifest.webmanifest','./css/style.css','./js/app.js','./icons/icon-192.png','./icons/icon-512.png'];
self.addEventListener('install',event=>{event.waitUntil((async()=>{const c=await caches.open(CACHE);await c.addAll(FILES);await Promise.all(['./wasm/esports.js','./wasm/esports.wasm'].map(async path=>{try{await c.add(path)}catch(_){}}))})());self.skipWaiting()});
self.addEventListener('activate',event=>{event.waitUntil(caches.keys().then(keys=>Promise.all(keys.filter(k=>k!==CACHE).map(k=>caches.delete(k)))));self.clients.claim()});
self.addEventListener('fetch',event=>{
 if(event.request.method!=='GET')return;
 const url=new URL(event.request.url);
 if(url.origin!==location.origin)return;
 event.respondWith(caches.match(event.request).then(hit=>hit||fetch(event.request).then(response=>{
  if(response.ok)caches.open(CACHE).then(c=>c.put(event.request,response.clone()));
  return response;
 }).catch(()=>caches.match('./index.html'))));
});
