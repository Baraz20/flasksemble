from flasksemble.sqlmodels import User
from flasksemble import app, db, bcrypt, mail
from flasksemble.forms import (RegistrationForm, LoginForm,
                               RequestResetForm, ResetPasswordForm,
                               AssemblerForm)
from flask import render_template, flash, redirect, url_for, request
from flask_login import login_user, current_user, logout_user, login_required
from flask_mail import Message
import os


@app.route("/")
@app.route("/home", methods=('GET', 'POST'))
def home():
    form = AssemblerForm()
    if form.validate_on_submit() and current_user.is_authenticated:
        flash('Sending Code!', 'success')
        # convert file CRLF -> LF
        fn = form.file_name.data
        stdout = process_asm(
            fn + app.config['ASSEMBLY_EXT'], form.body.data.replace('\n\r', '\n'))
        if stdout != '':
            return render_template('error.html', errors=stdout)
        os.system(f"{app.config['ASSEMBLER_PATH']}make deepclean")
        obj = get_file(fn + app.config['OBJECT_EXT'])
        ent = get_file(fn + app.config['ENTRY_EXT'])
        ext = get_file(fn + app.config['EXTERNAL_EXT'])
        return render_template('output.html', files=[obj, ent, ext])
    return render_template('home.html', form=form)


def get_file(fn: str):
    if not os.path.exists(app.config['ASSEMBLER_PATH'] + fn):
        return None
    with open(fn, 'r') as f:
        body = f.read()
    return body


def process_asm(fn: str, body: str):
    with open(fn, 'w') as f:
        f.write(body)
    stdout = os.popen(
        f"{app.config['ASSEMBLER_CMD']} {fn}").read()
    os.remove(fn)
    print("!!!!!!>>>>", f"{app.config['ASSEMBLER_CMD']} {fn}")
    return stdout


@app.route("/about")
def about():
    return render_template('about.html', title='About')


@app.route("/register", methods=("GET", "POST"))
def register():
    if current_user.is_authenticated:
        return redirect(url_for('home'))
    form = RegistrationForm()
    if form.validate_on_submit():
        hashed_password = bcrypt.generate_password_hash(
            form.password.data).decode('UTF-8')
        user = User(username=form.username.data,
                    email=form.email.data, password=hashed_password)
        db.session.add(user)
        db.session.commit()
        flash(f'Account created for {form.username.data}!', 'success')
        return redirect(url_for('login'))
    return render_template('register.html', title='Register', form=form)


@app.route("/login", methods=("GET", "POST"))
def login():
    if current_user.is_authenticated:
        return redirect(url_for('home'))
    form = LoginForm()
    if form.validate_on_submit():
        user = User.query.filter_by(email=form.email.data).first()
        if user and bcrypt.check_password_hash(user.password, form.password.data):
            login_user(user, remember=form.remember.data)
            next_page = request.args.get('next')
            return redirect(next_page) if next_page else redirect(url_for('home'))
        else:
            flash('Login Unsuccessful. Please check email and password', 'danger')
    return render_template('login.html', title='Login', form=form)


@app.route("/logout")
def logout():
    if not current_user.is_authenticated:
        flash('Logout Unsuccessful. Not logged in', 'danger')
        return redirect(url_for('home'))
    logout_user()
    flash('Logout successfully.', 'success')
    return redirect(url_for("home"))


def send_reset_email(user: User):
    token = user.get_reset_token()
    msg = Message(
        subject='Password Reset Request',
        sender='noreplay@flasksemble.com',
        recipients=[user.email],
        body=f'''Hey!
To reset you password, Press this link:{url_for('token_reset', token=token, _external=True)}

If you did not make this request then ignore the message above and no changes will be made,
Have a nice day! :)
''',
    )
    mail.send(msg)


@app.route('/reset_password', methods=('GET', 'POST'))
def request_reset():
    if current_user.is_authenticated:
        return redirect(url_for('home'))
    form = RequestResetForm()
    if form.validate_on_submit():
        user = User.query.filter_by(email=form.email.data).first()
        send_reset_email(user)
        flash('An email was sent to the given email for a Password Reset', 'info')
        return redirect(url_for('home'))
    return render_template('reset_password.html', title='Reset Password', form=form)


@app.route('/reset_password/<token>', methods=('GET', 'POST'))
def token_reset(token):
    if current_user.is_authenticated:
        return redirect(url_for('home'))
    user = User.verify_reset_token(token)
    if not user:
        flash('The token is invalid or expired', 'danger')
        return redirect(url_for('reset_password'))
    form = ResetPasswordForm()
    if form.validate_on_submit():
        hashed_password = bcrypt.generate_password_hash(
            form.password.data).decode('UTF-8')
        user.password = hashed_password
        db.session.commit()
        flash(f'Password has been changed successfully!', 'success')
        return redirect(url_for('login'))
    return render_template('reset_token.html', title='Reset Password', form=form)
