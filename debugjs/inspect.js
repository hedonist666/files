fs = require('fs')
frida = require('frida')
const cp = require('child_process')

console.log("{@_@} strasting investigation");

script_code = fs.readFileSync('payload.js')

console.log('injecting code: \n' + script_code)

dir = cp.execSync('pwd', (err, stdout, stderr) => {
  if (err) {
    console.log('errors_occured, exiting')
    return
  }
  return stdout;
})

child = cp.spawn(dir.toString().trim() + "/a.out", [], {
  stdio: "inherit"
})


console.log('starting attaching to process')

async function main() {

  let addr
  const message_handler = (message) => {
    if (message.type == 'send') {
      if (message.payload == null) {
          console.log('got null message')
      }
      else if (message.payload.startsWith('symbol')) {
        let str = message.payload.split(':')[1]
        str = cp.execSync('demangle ' + str)
        if (str.includes("operator")) {
          console.log(message.payload + '\ndemangled:' + str)
          console.log(addr)
          if (str.includes('ostream') && str.includes('<<') && str.includes('vector<char')) {
            console.log('FOUND WRITE OPERATOR')
            script.post({
              type: 'write',
              addr: addr.split(':')[1]
            })
          }
        }
      }
      else if (message.payload.startsWith('addr')) {
        addr = message.payload
      }
    }
    console.log('got message: ');
    console.log(message)
  }

  const file_dump = (message) => {
    console.log(message)
    fs.appendFile('frida.log', message.payload + '\n', err => {
      if (err) {
        console.error(err)
        return
      }
    })
  }

  const session = await frida.attach(child.pid)
  const script = await session.createScript(script_code)
  script.message.connect(file_dump)
  await script.load()
  await script.unload()
}

main().catch(e => {
  console.error(e)
})
