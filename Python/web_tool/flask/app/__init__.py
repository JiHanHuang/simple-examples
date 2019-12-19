from flask import Flask
from config import Config
from flask_login import LoginManager
#sql data base
from flask_sqlalchemy import SQLAlchemy
#data base migrate
from flask_migrate import Migrate
from flask_bootstrap import Bootstrap
from flask_socketio import SocketIO
import ssl

db = SQLAlchemy()
migrate = Migrate()
login = LoginManager()
login.login_view = 'auth.login'
login.login_message = 'Please log in to access this page.'
bootstrap = Bootstrap()
socketio = SocketIO()

def create_app(config_class=Config):
    app = Flask(__name__)
    app.config.from_object(config_class)

    db.init_app(app)
    migrate.init_app(app, db)
    login.init_app(app)
    bootstrap.init_app(app)
    socketio.init_app(app)

    # ... no changes to blueprint registration
    from app.errors import bp as errors_bp
    app.register_blueprint(errors_bp)

    from app.auth import bp as auth_bp
    app.register_blueprint(auth_bp, url_prefix='/auth')

    from app.tool import bp as tool_bp
    app.register_blueprint(tool_bp, url_prefix='/tool')

    from app.main import bp as main_bp
    app.register_blueprint(main_bp)

    ssl._create_default_https_context = ssl._create_unverified_context

    if not app.debug and not app.testing:
        pass
        # ... no changes to logging setup

    return app
