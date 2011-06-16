#include "messagebox.hpp"

using namespace MWGui;

MessageBoxManager::MessageBoxManager (WindowManager *windowManager)
{
    mWindowManager = windowManager;
    // defines
    mMessageBoxSpeed = 0.1;
}

void MessageBoxManager::onFrame (float frameDuration)
{
    std::vector<MessageBoxManagerTimer>::iterator it;
    for(it = mTimers.begin(); it != mTimers.end();)
    {
        it->current += frameDuration;
        if(it->current >= it->max)
        {
            removeMessageBox(it->messageBox);
            it = mTimers.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void MessageBoxManager::createMessageBox (const std::string& message)
{
    std::cout << "MessageBox: " << message << std::endl;
    
    MessageBox *box = new MessageBox(*this, message);
    
    removeMessageBox(message.length()*mMessageBoxSpeed, box);
    
    mMessageBoxes.insert(mMessageBoxes.begin(), box);
    int height = box->getHeight();
    std::vector<MessageBox*>::iterator it;
    
    int i = 0;
    for(it = mMessageBoxes.begin()+1; it != mMessageBoxes.end(); ++it)
    {
        if(i == 2) {
            delete (*it);
            mMessageBoxes.erase(it);
            break;
        }
        else {
            (*it)->update(height);
            height += (*it)->getHeight();
            i++;
        }
    }
}

void MessageBoxManager::createInteractiveMessageBox (const std::string& message, const std::vector<std::string>& buttons)
{
    std::cout << "interactive MessageBox: " << message << " - ";
    std::copy (buttons.begin(), buttons.end(), std::ostream_iterator<std::string> (std::cout, ", "));
    std::cout << std::endl;
    
    std::vector<MessageBox*>::iterator it = mMessageBoxes.begin();
    while(it != mMessageBoxes.end())
    {
        delete (*it);
        it = mMessageBoxes.erase(it);
    }
    mMessageBoxes.clear();
}

void MessageBoxManager::removeMessageBox (float time, MessageBox *msgbox)
{
    MessageBoxManagerTimer timer;
    timer.current = 0;
    timer.max = time;
    timer.messageBox = msgbox;
    
    mTimers.insert(mTimers.end(), timer);
}

bool MessageBoxManager::removeMessageBox (MessageBox *msgbox)
{
    std::vector<MessageBox*>::iterator it;
    for(it = mMessageBoxes.begin(); it != mMessageBoxes.end(); ++it)
    {
        if((*it) == msgbox)
        {
            delete (*it);
            mMessageBoxes.erase(it);
            return true;
        }
    }
    return false;
}

void MessageBoxManager::setMessageBoxSpeed (int speed)
{
    mMessageBoxSpeed = speed;
}




MessageBox::MessageBox(MessageBoxManager& parMessageBoxManager, const std::string& message)
  : Layout("openmw_messagebox_layout.xml")
  , mMessageBoxManager(parMessageBoxManager)
  , cMessage(message)
{
    // defines
    mFixedWidth = 300;
    mBottomPadding = 20;
    mNextBoxPadding = 20;
    
    getWidget(mMessageWidget, "message");
    
    mMessageWidget->setOverflowToTheLeft(true);
    mMessageWidget->addText(cMessage);
    
    MyGUI::IntSize size;
    size.width = mFixedWidth;
    size.height = 100; // dummy
    
    MyGUI::IntCoord coord;
    coord.left = 10; // dummy
    coord.top = 10; // dummy

    mMessageWidget->setSize(size);
    
    MyGUI::IntSize textSize = mMessageWidget->_getTextSize();
    size.height = mHeight = textSize.height + 20; // this is the padding between the text and the box
    
    mMainWidget->setSize(size);
    size.width -= 5; // this is to center the text (see messagebox_layout.xml, Widget type="Edit" position="-2 -3 0 0")
    mMessageWidget->setSize(size);
    
    update(0);
}

void MessageBox::update (int height)
{
    MyGUI::IntSize gameWindowSize = mMessageBoxManager.mWindowManager->getGui()->getViewSize();
    MyGUI::IntCoord coord;
    coord.left = (gameWindowSize.width - mFixedWidth)/2;
    coord.top = (gameWindowSize.height - mHeight - height - mBottomPadding);
    
    MyGUI::IntSize size;
    size.width = mFixedWidth;
    size.height = mHeight;
    
    mMainWidget->setCoord(coord);
    mMainWidget->setSize(size);
    mMainWidget->setVisible(true);
}

int MessageBox::getHeight ()
{
    return mHeight+mNextBoxPadding; // 20 is the padding between this and the next MessageBox
}