from app import db

data_type_list = [('str', 'String'), ('json', 'Json')]

class Service(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(64), index=True, unique=True)
    data = db.Column(db.String(1024))
    data_type = db.Column(db.String(64))

    def __repr__(self):
        return '{}'.format(self.name)