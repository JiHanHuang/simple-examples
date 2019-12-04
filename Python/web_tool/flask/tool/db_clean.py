import sys
sys.path.append('../')
from app import db
from app.auth.users import User, Post

if __name__ == '__main__':
    users = User.query.all()
    for u in users:
        db.session.delete(u)
    posts = Post.query.all()
    for p in posts:
        db.session.delete(p)
    db.session.commit()
    print('user, post clean finish.')