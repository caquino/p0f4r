#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netinet/in.h>
#include "ruby.h"
#include "p0f-query.h"
#include "types.h"


static VALUE
t_init (VALUE self,VALUE socketpath)
{
	struct sockaddr_un x;
	_s32 sock;

	sock = socket(PF_UNIX,SOCK_STREAM,0);
	if (sock < 0) rb_raise (rb_eTypeError, "socket error");
	x.sun_family=AF_UNIX;
	strncpy(x.sun_path,STR2CSTR(socketpath),100);
	if (connect(sock,(struct sockaddr*)&x,sizeof(x))) rb_raise (rb_eTypeError, "connect error");



	return self;
}

static VALUE
t_query (VALUE self, VALUE sourceip, VALUE sourceport, VALUE destip, VALUE destport)
{
struct p0f_query p;
struct p0f_response r;
_u32 s,d,sp,dp;
p.magic = QUERY_MAGIC;
p.id = 0x12345678;
p.src_ad = s;
p.dst_ad = d;
p.src_port = sp;
p.dst_port = dp;
 
 if (write(sock,&p,sizeof(p)) != sizeof(p))
 fatal("Socket write error (timeout?).\n");
  
	if (read(sock,&r,sizeof(r)) != sizeof(r))
	fatal("Response read error (timeout?).\n");
	 
	 if (r.magic != QUERY_MAGIC)
	 fatal("Bad response magic.\n");
	  
		if (r.type == RESP_BADQUERY)
		fatal("P0f did not honor our query.\n");




		 
		 if (r.type == RESP_NOMATCH) {
		 printf("This connection is not (no longer?) in the cache.\n");
		 exit(3);
		 }
		  
			if (!r.genre[0]) {
			printf("Genre and OS details not recognized.\n");
			} else {
			printf("Genre : %s\n",r.genre);
			printf("Details : %s\n",r.detail);
			if (r.dist != -1) printf("Distance : %d hops\n",r.dist);
			}
			 
			 if (r.link[0]) printf("Link : %s\n",r.link);
			 if (r.tos[0]) printf("Service : %s\n",r.tos);
			  
				if (r.uptime != -1) printf("Uptime : %d hrs\n",r.uptime);
				 
				 if (r.score != NO_SCORE)
				 printf("M-Score : %d%% (flags %x).\n",r.score,r.mflags);
				  
					if (r.fw) printf("The host is behind a firewall.\n");
					if (r.nat) printf("The host is behind NAT or such.\n");
}


static VALUE
t_close (VALUE self)
{
shutdown(sock,2);
close(sock);
}

static VALUE
t_ipv4 (VALUE self,VALUE ipaddress)
{
  rb_iv_set (self, "@valid", (inet_addr(STR2CSTR(ipaddress)) == INADDR_NONE) ? Qfalse : Qtrue);
	return self;
}

static VALUE t_valid(VALUE self, VALUE anObject){
	return  rb_iv_get(self, "@valid");
}


VALUE cp0f;  

void
Init_p0f()
{
	cp0f = rb_define_class("p0f", rb_cHash);
	rb_define_method (cp0f,"initialize", t_init, 0);
	rb_define_method (cp0f,"IPv4", t_ipv4, 1);
	rb_define_method (cp0f,"valid?", t_valid, 0);
}
