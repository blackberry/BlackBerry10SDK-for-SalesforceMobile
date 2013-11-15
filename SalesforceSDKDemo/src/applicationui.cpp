#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Container>
#include <bb/cascades/Button>
#include <bb/cascades/Label>
#include <bb/data/JsonDataAccess>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <memory>

#include <core/SFGlobal.h>
#include <core/SFAuthenticationManager.h>
#include <core/SFResult.h>
#include <core/SFGenericTask.h>
#include <core/SFNetworkAccessTask.h>
#include <core/SFIdentityData.h>
#include <rest/SFRestAPI.h>
#include <rest/SFRestResourceTask.h>
#include <rest/SFRestRequest.h>
#include <oauth/SFOAuthInfo.h>
#include <oauth/SFOAuthCredentials.h>

//TODO To use the Salesforce SDK, you need to assign proper values for following constants
QString const sf::SFRemoteAccessConsumerKey = "3MVG9y6x0357HleejRitlDvJj0_GfIS.kLS.OK0LCO6ifB8NxYkfM_0ZcT.J_wY25w4AJu2RIOEvv4INMaamm";
QString const sf::SFOAuthRedirectURI = "testsfdc:///mobilesdk/detect/oauth/done";
QString const sf::SFDefaultRestApiVersion = "/v28.0";

using namespace bb::cascades;
using namespace sf;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) : sf::SFAbstractApplicationUI(app)
{
	//for debugging purpose only
	qDebug()<<"organization name "<< QCoreApplication::organizationName();
	qDebug()<<"application name "<< QCoreApplication::applicationName();

    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    if(!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }
    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    //Livan, connect some slots
    Button *button = root->findChild<Button*>("cplusplusTestBtn");
    connect(button, SIGNAL(clicked()), this, SLOT(onCPlusPlusTestClicked()));

    button = root->findChild<Button*>("qmlTestBtn");
    connect(button, SIGNAL(clicked()), this, SLOT(onQmlTestClicked()));

    button = root->findChild<Button*>("remokeAccessBtn");
    connect(button, SIGNAL(clicked()), this, SLOT(onRevokeAccessClicked()));

    button = root->findChild<Button*>("revokeAllTokens");
    connect(button, SIGNAL(clicked()), this, SLOT(onRevokeAllTokensClicked()));

    button = root->findChild<Button*>("oauthLoginBtn");
    connect(button, SIGNAL(clicked()), this, SLOT(onOAuthLoginClicked()));

    button = root->findChild<Button*>("oauthLogoutBtn");
    connect(button, SIGNAL(clicked()), this, SLOT(onOAuthLogoutClicked()));

    mConsoleLabel = root->findChild<Label*>("consoleLbl");
    mOAuthLabel = root->findChild<Label*>("oAuthLbl");

    //connect slots for authentication
	SFAuthenticationManager* authManager = SFAuthenticationManager::instance();

	//connect slots
	connect(authManager, SIGNAL(SFOAuthFlowSuccess(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowSuccess(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowCanceled(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowCanceled(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFLoginHostChanged()), this, SLOT(onSFLoginHostChanged()), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowSuccess(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowSuccess(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowFailure(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowFailure(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFUserLoggedOut()), this, SLOT(onSFUserLoggedOut()), Qt::UniqueConnection);


	updateOAuthLabel();
    // Set created root object as the application scene
    app->setScene(root);
}

void ApplicationUI::updateOAuthLabel(){
	SFAuthenticationManager* authManager = SFAuthenticationManager::instance();
	QString oauthTokens = QString("Access Token: %1 \nRefereshToken: %2 \nInstanceUrl: %3 \nDisplayName: %4").arg(
			authManager->getCredentials()->getAccessToken(),
			authManager->getCredentials()->getRefreshToken(),
			authManager->getCredentials()->getInstanceUrl().toString(),
			authManager->getIdData()->getDisplayName());
	mOAuthLabel->setText(oauthTokens);
}


void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("SalesForceOAuth_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

/************************
 * UI Slots
 ************************/
void ApplicationUI::onCPlusPlusTestClicked() {
	qDebug() << "[C++] Performing SOQL: Select Id, Name From Account Limit 5";

	QStringList fields = QStringList() << "Id" << "Name";
	QString query = SFRestAPI::instance()->generateSOQLQuery(fields, "Account", "", QStringList(), "", QStringList(), 5);
	SFRestRequest * request = SFRestAPI::instance()->requestForQuery(query);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onRestResultReady(sf::SFResult*)));
}

void ApplicationUI::onQmlTestClicked() {
	//this should be handled in QML
}

void ApplicationUI::onRevokeAccessClicked() {
	const SFOAuthCredentials * credentials = SFRestAPI::instance()->currentCredentials();
	QString accessToken = credentials ? credentials->getAccessToken() : QString();
	if (accessToken.isNull() || accessToken.isEmpty()) {
		return;
	}

	qDebug() << "Revoking access token:" << accessToken;

	QString path = QString("https://%1/services/oauth2/revoke?token=%2").arg(SFAuthenticationManager::instance()->loginHost(), accessToken);
	SFNetworkAccessTask *task = new SFNetworkAccessTask(getSharedNetworkAccessManager(), path, HTTPMethod::HTTPGet);
	connect(task, SIGNAL(taskFinished(sf::SFGenericTask*)), this, SLOT(onNetworkTaskFinished(sf::SFGenericTask*)));
	task->startTaskAsync(this, SLOT(onNetworkResultReady(sf::SFResult*)));
}

void ApplicationUI::onRevokeAllTokensClicked() {
	const SFOAuthCredentials * credentials = SFRestAPI::instance()->currentCredentials();
	QString refreshToken = credentials ? credentials->getRefreshToken() : QString();
	if (refreshToken.isNull() || refreshToken.isEmpty()) {
		return;
	}
	//revoking refresh token will invalidate access token as well
	qDebug() << "Revoking refresh token" << refreshToken;

	QString path = QString("https://%1/services/oauth2/revoke?token=%2").arg(SFAuthenticationManager::instance()->loginHost(), refreshToken);
	SFNetworkAccessTask *task = new SFNetworkAccessTask(getSharedNetworkAccessManager(), path, HTTPMethod::HTTPGet);
	task->startTaskAsync(this, SLOT(onNetworkResultReady(sf::SFResult*)));
}

/************************
 * Network test slots
 ************************/
void ApplicationUI::onRestResultReady(sf::SFResult *result) {
	QString msg("REST Result:\n");
	qDebug() << "REST Result:" << result;
	if (!result->hasError()) {
		qDebug() << "payload:" << result->payload<QVariant>();
		bb::data::JsonDataAccess jda;
		QByteArray buffer;
		jda.saveToBuffer(result->payload<QVariant>(), &buffer);
		msg.append("payload: ").append(QString(buffer));
	} else {
		qDebug() << "Error code:" << result->code() << "Message:" << result->message();
		msg.append("Error code: ").append(QString::number(result->code())).append(" Message: ").append(result->message());
	}
	mConsoleLabel->setText(msg);
}

void ApplicationUI::onNetworkResultReady(sf::SFResult *result) {
	QString msg("Network Result:\n");
	qDebug() << "Network Result:" << result;
	if (!result->hasError()) {
		QNetworkReply *reply = result->payload<QNetworkReply*>();
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
		QString content = QString(reply->readAll());

		if (content.isNull() || content.trimmed().isEmpty()) {
			qDebug() << "Reply:" << statusCode << reason;
			msg.append("Reply: ").append(QString::number(statusCode)).append(" - ").append(reason);
		} else {
			qDebug() << "Reply:" << content;
			msg.append("Reply: ").append(content);
		}
	} else {
		qDebug() << "Error code:" << result->code() << "Message:" << result->message();
		msg.append("Error code: ").append(QString::number(result->code())).append(" Message: ").append(result->message());
	}
	mConsoleLabel->setText(msg);
}

void ApplicationUI::onNetworkTaskFinished(sf::SFGenericTask *task) {
	qDebug() << "Task finished:" << task->status();
}

/************************
 * oAuth test slots
 ************************/
void ApplicationUI::onOAuthLoginClicked(){
	mOAuthLabel->setText("authenticating");
	qDebug() << "OAuth Login Clicked";
	SFAuthenticationManager* authManager = SFAuthenticationManager::instance();
	authManager->login();
}
void ApplicationUI::onOAuthLogoutClicked(){
	qDebug() << "OAuth logout Clicked";
	SFAuthenticationManager* authManager = SFAuthenticationManager::instance();
	authManager->logout();
}

void ApplicationUI::onSFOAuthFlowSuccess(SFOAuthInfo* info){
	qDebug()<<"the authentication flow was "<<info->getAuthType();

	qDebug()<<"onSFOAuthFlowSuccess access token is:";
	qDebug()<<SFAuthenticationManager::instance()->getCredentials()->getAccessToken();

	qDebug()<<"onSFOAuthFlowSuccess instance url is:";
	qDebug()<<SFAuthenticationManager::instance()->getCredentials()->getInstanceUrl();

	updateOAuthLabel();
}

void ApplicationUI::onSFOAuthFlowCanceled(SFOAuthInfo* info){
	qDebug()<<"the canceled authentication flow was "<<info->getAuthType();
	updateOAuthLabel();
}

void ApplicationUI::onSFOAuthFlowFailure(SFOAuthInfo* info){
	qDebug()<<"the failed authentication flow was "<<info->getAuthType();
	updateOAuthLabel();
}

void ApplicationUI::onSFUserLoggedOut(){
	qDebug()<<"received user logged out signal";
	//recommended: automatically presents the login screen again
	SFAuthenticationManager::instance()->login();
	updateOAuthLabel();
}
void ApplicationUI::onSFLoginHostChanged(){
	qDebug()<<"received host changed signal";
	SFAuthenticationManager::instance()->login();
	updateOAuthLabel();
}
