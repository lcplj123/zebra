import tornado.ioloop
import tornado.web
import torndb

http_port = 5678
my_host = "127.0.0.1:3306"
my_db = "zebra"
my_user = "zreportor"
my_pwd = "1234Qwerx"


class IndexHandler(tornado.web.RequestHandler):
	def get(self):
		self.write("Hello World!")

class UpdateHandler(tornado.web.RequestHandler):
	def get(self):
		sql = self.get_argument("sql")
		key = self.get_argument("op")
		if key != "update":
			self.write("{'status':500}")
			return

		db.execute(sql);
		self.write("{'status':200}")
	def post(self):
		sql = self.get_argument("sql")
		key = self.get_argument("op")
		print(sql)
		print(key)
		if key != "update":
			self.write("{'status':500}")
			return
		db.execute(sql);
		self.write("{'status':200}")

class ProcessHandler(tornado.web.RequestHandler):
	def get(self):
		ip = self.get_argument("ip")
		sql =' select processList from machineinfo where ip = %s'
		result = db.get(sql,ip)
		if result is None:
			self.write("")
		else:
			self.write(result.get("processList",""))
		print(result)
	def post(self):
		ip = self.get_argument("ip")
		sql = 'select processList from machineinfo where ip = %s'
		result = db.get(sql,ip)
		if result is None:
			self.write("");
		else:
			self.write(result.get("processList",""))
		print(result)

			
if __name__ == "__main__":
	handlers = [
		(r"/",IndexHandler),
		(r"/monitor/hardware",UpdateHandler),
		(r"/monitor/request/proclist",ProcessHandler),
	]

	db = torndb.Connection(
		host = my_host,
		database = my_db,
		user = my_user,
		password = my_pwd,
	)
	
	app = tornado.web.Application(handlers)
	app.listen(http_port)
	tornado.ioloop.IOLoop.instance().start()
