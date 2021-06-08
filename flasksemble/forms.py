from wtforms.fields.simple import TextField
from flasksemble.sqlmodels import User
from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, SubmitField, BooleanField, TextAreaField
from wtforms.validators import DataRequired, Length, Email, EqualTo, ValidationError


class RegistrationForm(FlaskForm):
    username = StringField(
        'Username',
        validators=[DataRequired(), Length(min=2, max=20)])
    email = StringField(
        'Email',
        validators=[DataRequired(), Email()])
    password = PasswordField(
        'Password',
        validators=[DataRequired()])
    confirm_password = PasswordField(
        'Confirm Password',
        validators=[DataRequired(), EqualTo('password')])
    submit = SubmitField('Sign Up')

    def validate_username(self, username):
        if User.query.filter_by(username=username.data).first() is not None:
             raise ValidationError("This username is already taken!")
    
    def validate_email(self, email):
        if User.query.filter_by(username=email.data).first() is not None:
             raise ValidationError("This email is already taken!")


class LoginForm(FlaskForm):
    email = StringField(
        'Email',
        validators=[DataRequired(), Email()])
    password = PasswordField(
        'Password',
        validators=[DataRequired()])
    remember = BooleanField('Remember Me')
    submit = SubmitField('Login')

class RequestResetForm(FlaskForm):
    email = StringField(
        'Email',
        validators=[DataRequired(), Email()])
    submit = SubmitField('Request Password Reset')
    
    def validate_email(self, email):
        if User.query.filter_by(email=email.data).first() is None:
             raise ValidationError("There is no user with that email!")

class ResetPasswordForm(FlaskForm):
    password = PasswordField(
        'Password',
        validators=[DataRequired()])
    confirm_password = PasswordField(
        'Confirm Password',
        validators=[DataRequired(), EqualTo('password')])
    submit = SubmitField('Set New Password')

class AssemblerForm(FlaskForm):
    file_name = StringField('File Name', validators=[DataRequired()])
    body = TextAreaField('Code', validators=[DataRequired()])
    submit = SubmitField('Send Code')

    