# portscanner

A simple port scanner using raw sockets in C

## Build

`make clean` <br>
`make`

## Usage

`./portscanner <target_ip_address> <connection_timeout (seconds)> `

## Example

`./portscanner 127.0.0.1 1`

### Output

```
22      open
25      open
587     open
631     open
1716    open
5432    open
5433    open
5434    open
9050    open
46108   open
48580   open
```
