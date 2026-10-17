#!/usr/bin/env python3
"""Local UI server. OS demonstrations are implemented in C and executed on Linux/WSL."""
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from urllib.parse import urlparse
import json, os, subprocess, threading, time

ROOT=os.path.dirname(os.path.abspath(__file__))
HOST,PORT="127.0.0.1",8000
RUN_LOCK=threading.Lock()
ALLOWED={"tree","fork","exec","wait","signal","ipc"}

def run_command(args, timeout=8):
    try:
        p=subprocess.run(args,capture_output=True,text=True,timeout=timeout,cwd=ROOT)
        return {"returncode":p.returncode,"stdout":p.stdout[-12000:],"stderr":p.stderr[-4000:]}
    except subprocess.TimeoutExpired as e:
        return {"returncode":124,"stdout":(e.stdout or "") if isinstance(e.stdout,str) else "", "stderr":"Demo timed out."}
    except Exception as e:
        return {"returncode":1,"stdout":"","stderr":str(e)}

def process_list():
    p=subprocess.run(["ps","-eo","pid=,ppid=,stat=,comm=,args="],check=True,capture_output=True,text=True)
    out=[]
    for line in p.stdout.splitlines():
        parts=line.strip().split(None,4)
        if len(parts)<4: continue
        try: pid,ppid=int(parts[0]),int(parts[1])
        except ValueError: continue
        out.append({"pid":pid,"ppid":ppid,"state":parts[2],"name":parts[3],"command":parts[4] if len(parts)>4 else parts[3]})
    return out

class Handler(SimpleHTTPRequestHandler):
    def __init__(self,*a,**kw): super().__init__(*a,directory=ROOT,**kw)
    def reply(self,data,status=200):
        b=json.dumps(data).encode()
        self.send_response(status); self.send_header("Content-Type","application/json; charset=utf-8")
        self.send_header("Content-Length",str(len(b))); self.send_header("Cache-Control","no-store"); self.end_headers(); self.wfile.write(b)
    def do_GET(self):
        path=urlparse(self.path).path
        if path=="/api/processes":
            try:
                ps=process_list(); self.reply({"processes":ps,"count":len(ps)})
            except Exception as e: self.reply({"error":str(e)},500)
        elif path=="/api/health":
            self.reply({"status":"ok","compiler":run_command(["gcc","--version"],2)["returncode"]==0})
        else:
            if path=="/": self.path="/index.html"
            super().do_GET()
    def do_POST(self):
        path=urlparse(self.path).path
        if path!="/api/demo": self.reply({"error":"Not found"},404); return
        try:
            n=int(self.headers.get("Content-Length","0"))
            data=json.loads(self.rfile.read(min(n,2000)).decode() or "{}")
            demo=data.get("demo","")
            if demo not in ALLOWED: self.reply({"error":"Unknown demo"},400); return
            with RUN_LOCK:
                build=run_command(["gcc","-Wall","-Wextra","-std=c11","-o","os_demo","os_demo.c"],10)
                if build["returncode"]!=0:
                    self.reply({"title":demo,"returncode":build["returncode"],"stdout":"","stderr":"Compilation failed:\\n"+build["stderr"]},500); return
                args=["./os_demo",demo]
                # signal and IPC demonstrations are bounded by the C program.
                result=run_command(args,8)
                result["title"]=demo
                self.reply(result)
        except Exception as e: self.reply({"error":str(e)},500)
    def log_message(self,fmt,*args): print("[server]",fmt%args)

if __name__=="__main__":
    print("Linux Process Tree Visualizer (C + Linux)")
    print(f"Open http://{HOST}:{PORT} in your browser")
    print("Keep this terminal open. Press Ctrl+C to stop.")
    ThreadingHTTPServer((HOST,PORT),Handler).serve_forever()
