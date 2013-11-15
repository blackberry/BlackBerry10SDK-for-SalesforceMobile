#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <core/SFAbstractApplicationUI.h>

/* forward declarations */
namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
        class Label;
    }
}

namespace sf {
	class SFGenericTask;
	class SFResult;
	class SFOAuthInfo;
}

class QTranslator;

using namespace bb::cascades;
using namespace sf;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI : public sf::SFAbstractApplicationUI
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }

private slots:
    void onSystemLanguageChanged();

    /* slots for UI components */
    void onCPlusPlusTestClicked();
    void onQmlTestClicked();
    void onRevokeAccessClicked();
    void onRevokeAllTokensClicked();
    void onOAuthLoginClicked();
    void onOAuthLogoutClicked();

    /* Authentication Related */
    void onSFOAuthFlowSuccess(SFOAuthInfo* info);
    void onSFOAuthFlowCanceled(SFOAuthInfo* info);
    void onSFOAuthFlowFailure(SFOAuthInfo* info);
    void onSFUserLoggedOut();
    void onSFLoginHostChanged();

    /* For Testing */
    void onRestResultReady(sf::SFResult *result);
    void onNetworkResultReady(sf::SFResult *result);
    void onNetworkTaskFinished(sf::SFGenericTask *task);

private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    Label *mConsoleLabel;
    Label *mOAuthLabel;

    void updateOAuthLabel();
};
#endif /* ApplicationUI_HPP_ */
