

int accept (int fd, struct sockaddr * addr, socklen_t * len) {
	typedef int (*libcall)(int,struct sockaddr *, socklen_t *);
}

int socket (int domain, int type, int protocol) {
	typedef int (*libcall)(int,int,int);
}

int close (int fd) {
	typedef int (*libcall)(int);
}

int connect (int fd, const struct sockaddr * addr, socklen_t len) {
	typedef int (*libcall)(int,const struct sockaddr *,socklen_t);
}


int bind (int fd, const struct sockaddr * addr, socklen_t len) {
	typedef int (*libcall)(int,const struct sockaddr *, socklen_t);
}

ssize_t sendto (int fd, const void * buf, size_t size, int flags,
		const struct sockaddr * addr, socklen_t len) {
	typedef ssize_t (*libcall)(int,const void *,size_t,int,const struct sockaddr *,socklen_t);
}

ssize_t send (int fd, const void * buf, size_t size, int flags) {
	typedef ssize_t (*libcall)(int,const void *,size_t,int);
}

ssize_t writev (int fd, const struct iovec * io, int iocnt) {
	typedef ssize_t (*libcall)(int,const struct iovec *,int);
}

ssize_t write (int fd, const void * buf, size_t size) {
	typedef ssize_t (*libcall)(int,const void *,size_t);
}

ssize_t sendmsg (int fd, const struct msghdr * msg, int flags) {
	typedef ssize_t (*libcall)(int,const struct msghdr *,int);
}

ssize_t recvfrom (int fd, void * buf, size_t size, int flags, struct sockaddr * addr, socklen_t * len) {
	typedef ssize_t (*libcall)(int,void *,size_t,int,struct sockaddr *,socklen_t *);
}

ssize_t recv (int fd, void * buf, size_t size, int flags) {
	typedef ssize_t (*libcall)(int,void *,size_t,int);
}

ssize_t read (int fd, void * buf, size_t size) {
	typedef ssize_t (*libcall)(int,void *,size_t);
}

ssize_t recvmsg (int fd, struct msghdr * msg, int flags) {
	typedef ssize_t (*libcall)(int,struct msghdr *,int);
}
