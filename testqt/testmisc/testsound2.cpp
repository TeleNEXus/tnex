#include <QApplication>
#include <QtWidgets>
#include <qaudio.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <QScrollArea>
#include <qscrollarea.h>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPixmap>
#include <QMovie>
#include <QPushButton>
#include <QDebug>
#include <QTouchDevice>
#include <QList>
#include <QMouseEvent>
#include <QLineEdit>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QBuffer>


class LQBuffer : public QBuffer
{
public:
  using QBuffer::QBuffer;


  virtual bool atEnd() const override
  {
    return false;
  }

  virtual bool seek(qint64 pos) override
  {
    QBuffer::seek(pos);
    return true;
  }

  virtual qint64	readData(char *data, qint64 len) override
  {
    qint64 rd1 = QBuffer::readData(data, len);

    if(rd1 == len)
    {
      return rd1;
    }

    QBuffer::seek(44);
    /* seek(44 + (len - rd1)); */
    qint64 rd2 = QBuffer::readData(&data[rd1], len-rd1);

    return len;

  }
};





class LQPlaySound : QObject
{
public:
  enum ELoop{ Infinite = -1 };
private:
  QIODevice* mpDevice;
  int mLoops;
  int mLoopsCounter;
  QAudioOutput* mpAOut;
public:
  LQPlaySound() = delete;
  explicit LQPlaySound(QIODevice* _device, QObject *parent = nullptr) :
    QObject(parent)
    ,mpDevice(_device)
    ,mLoops(0)
    ,mLoopsCounter(0)
    ,mpAOut(nullptr)
  {
    if(!mpDevice->isOpen()) return;

    QByteArray wav_header_data = mpDevice->read(44);

    if(wav_header_data.isNull()) 
    {
      return;
    }

    QString descr_riff = QString::fromLatin1(wav_header_data.data(), 4);
    QString descr_wave = QString::fromLatin1(&wav_header_data.data()[8], 4);

    if(descr_riff != QString("RIFF"))  return;
    if(descr_wave != QString("WAVE"))  return;

    quint16 num_channels = *((quint16*)(&(wav_header_data.data()[22])));
    quint32 sample_rate = *((quint32*)(&(wav_header_data.data()[24])));
    /* quint32 byte_rate = *((quint32*)(&(data.data()[28]))); */
    quint16 bits_per_sample = *((quint16*)(&(wav_header_data.data()[34])));

    QAudioFormat format;

    format.setChannelCount(num_channels);
    format.setSampleRate(sample_rate);
    format.setSampleSize(bits_per_sample);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    mpAOut = new QAudioOutput(format);

    QObject::connect(mpAOut, &QAudioOutput::stateChanged,
        [this](QAudio::State _state)
        {
          switch(_state)
          {
          case QAudio::State::IdleState:

            if(mLoops == static_cast<int>(ELoop::Infinite))
            {
              /* startPrivate(); */

    mpDevice->seek(44);
    mpAOut->resume();

              break;
            } 

            if(mLoops == 0) 
            {
              mpAOut->stop();
              break;
            }

            mLoopsCounter--;
            if(mLoopsCounter >= 0)
            {
              /* startPrivate(); */
    mpDevice->seek(44);
    mpAOut->resume();
            }
            else
            {
              mLoopsCounter = 0;
              mpAOut->stop();
            }
            break;

          default:
            break;
          }

        });

  }

  virtual ~LQPlaySound()
  {
    if(mpAOut != nullptr) return;
    mpAOut->stop();
    mpAOut->deleteLater();
  }

  QIODevice* device() const {return mpDevice;}

  bool isFinished() const
  { 
    if(mpAOut == nullptr) return true;
    return  (mpAOut->state() == QAudio::State::StoppedState) ? (true) : (false);
  }
  int loops() const { return mLoops; }
  int loopsRemaining() const 
  {
    if(mLoops == ELoop::Infinite) return ELoop::Infinite;
    return mLoopsCounter;
  }

  void setLoops(int _number)
  {
    if(_number < 0) 
    {
      mLoops = ELoop::Infinite;
      mLoopsCounter = 0;
    }
    else 
    {
      mLoops = _number;
      mLoopsCounter = mLoops;
    }
  }

  void play()
  {
    if(mpAOut == nullptr) return;
    startPrivate();
    mLoopsCounter = mLoops;
  }

  void stop()
  {
    if(mpAOut == nullptr) return;
    mpAOut->stop();
    mLoopsCounter = 0;
  }

private:
  void startPrivate()
  {
    mpDevice->seek(44);
    mpAOut->start(mpDevice);
  }
};

static void test();

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QFrame* frame = new QFrame;
  QPushButton* button_play = new QPushButton("Play");
  QPushButton* button_play_loop = new QPushButton("Play loop");
  QPushButton* button_stop = new QPushButton("Stop");
  QLineEdit* loops_edit = new QLineEdit();
  QVBoxLayout* layout = new QVBoxLayout;

  if(QAudioDeviceInfo::defaultOutputDevice().isNull())
  {
    qDebug() << "Can't find default audio device.";
  }
  else
  {
    qDebug() << QAudioDeviceInfo::defaultOutputDevice().deviceName();
  }

  qDebug().noquote() << "Audio devices info: ";
  auto avail = QAudioDeviceInfo::availableDevices(QAudio::Mode::AudioOutput);
  

  for(int i = 0; i < avail.size(); i++)
  {
    qDebug().noquote() << avail[i].deviceName();
  }


  /* QFile sound_file("../sirena1.wav"); */
  /* QFile sound_file("../sirena_001.wav"); */
  QFile sound_file("../discord-sounds.wav");
  /* QFile sound_file("../sirena_003.wav"); */
  /* QFile sound_file("../beep-07a.wav"); */
  if(!sound_file.open(QFile::OpenModeFlag::ReadOnly)) 
  {
    qDebug() << "Can't open sound file.";
  }

  /* QByteArray sound_data = sound_file.readAll(); */

  /* LQBuffer sound_buffer(&sound_data); */
  /* sound_buffer.open(QIODevice::ReadOnly); */

  /* LQPlaySound* play_sound = new LQPlaySound(&sound_buffer); */
  LQPlaySound* play_sound = new LQPlaySound(&sound_file);

  /* play_sound->setLoops(3); */
  qDebug() << "Play loops = " << play_sound->loops();

  QObject::connect(loops_edit, &QLineEdit::editingFinished,
      [play_sound, loops_edit]()
      {
        if(loops_edit->text().isNull()) return;
        if(loops_edit->text() == "") return;
        if(loops_edit->text().toLower() == "inf")
        {
          play_sound->setLoops(LQPlaySound::Infinite);
          qDebug() << "Set inf";
          return;
        }
        bool flag = false;
        int loops = loops_edit->text().toInt(&flag);
        if(flag)
        {
          play_sound->setLoops(loops);
        }
        else
        {
          loops_edit->setText(QString("%1").arg(play_sound->loops()));
        }
      });

  QObject::connect(button_play, &QPushButton::pressed,
      [play_sound]()
      {
        qDebug() << "-----------------Play sound";

        /* file.seek(44); */
        /* audioOutput->start(&file); */
        play_sound->play();
      });

  QObject::connect(button_play_loop, &QPushButton::pressed,
      []()
      {
      });

  QObject::connect(button_stop, &QPushButton::pressed,
      [play_sound]()
      {
        qDebug() << "-----------------Stop sound";
        /* audioOutput->stop(); */
        play_sound->stop();
      });


  layout->addWidget(loops_edit);
  layout->addWidget(button_play);
  layout->addWidget(button_play_loop);
  layout->addWidget(button_stop);
  frame->setLayout(layout);
  frame->show();
  return app.exec();
}
//------------------------------------------------
static void test()
{ 

  static QFile file("../sirena1.wav");
  qDebug() << file.open(QIODevice::ReadOnly);
  file.seek(44); // Length of wav header

  static QAudioFormat format;

  format.setSampleSize(16);
  format.setSampleRate(44100);
  //format.setChannelCount(1);    // mono
  format.setChannelCount(2); // stereo
  format.setCodec("audio/pcm");
  format.setByteOrder(QAudioFormat::LittleEndian);
  format.setSampleType(QAudioFormat::SignedInt);

  static QAudioOutput audioOutput(format);
  audioOutput.start(&file);
  qDebug() << audioOutput.state();
}

