send('zdarov')
send('working in: ' + Process.platform);

var v = 666
var parse_args_offset = 0x000025d9 //r2
var base = Module.getBaseAddress('a.out')
var paddr = base.add(parse_args_offset)

send('base: ' + base)
send('pa: ' + paddr)


Interceptor.attach(paddr, {
  onEnter: function(args) {
    send('parse_args');
  },
  onLeave: function(ret) {
    send('left))')
    console.log('check')
  }
})

var printf = Module.getExportByName('libc-2.30.so', 'printf')

send(printf)
console.log(hexdump(printf, {
  offset: 0,
  length: 128,
  header: true,
  ansi: true
}))

Interceptor.attach(printf, {
  onEnter: function(args) {
    console.log('from printf');
  },
  onLeave: function(ret) {
    console.log('left from printf')
  }
})

Interceptor.flush()
