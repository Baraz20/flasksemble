from datetime import datetime
from flasksemble import db, login_manager, app
from flask_login import UserMixin
from itsdangerous import TimedJSONWebSignatureSerializer as JWS

@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))


class Log(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    descreption = db.Column(db.String(200), nullable=False)
    # utcnow is universal time
    date = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'))

    def __repr__(self) -> str:
        return f"Log('{self.data}', '{self.descreption}', '{self.date}', '{self.user_id}')"


class User(db.Model, UserMixin):
    id = db.Column(db.Integer,     primary_key=True)
    username = db.Column(db.String(20),  unique=True,    nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    password = db.Column(db.String(60),  nullable=False)
    logs = db.relationship(Log.__name__, backref='user', lazy=True)

    def get_reset_token(self, expires_sec=10*60): # 10 minutes
        s = JWS(app.config['SECRET_KEY'], expires_sec)
        return s.dumps({'user_id': self.id}).decode('utf-8')

    @staticmethod
    def verify_reset_token(token):
        s = JWS(app.config['SECRET_KEY'])
        try:
            user_id = s.loads(token)['user_id']
        except:
            return None
        return User.query.get(user_id)

    def __repr__(self) -> str:
        return f"User('{self.username}', '{self.email}')"
