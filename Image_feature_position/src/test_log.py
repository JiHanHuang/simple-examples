from log import log

l=log(out_level=1,log_level=1)
list=[1,2,3,4]
dic={1:'a',2:'b'}
l.debug("aaaa")
l.debug("aaa",list)
l.debug("aa",list,dic)

l.info("bbbb")
l.info("bbb",list)
l.info("bb",list,dic)

l.warning("cccc")
l.warning("ccc",list)
l.warning("cc",list,dic)
