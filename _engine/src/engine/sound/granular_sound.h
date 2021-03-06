#ifndef __GRANULAR_SOUND__
#define __GRANULAR_SOUND__

/**
* Synth� granulaire.
* Lui charger un fichier de base ou il pioche les grains
* utiliser setGrainParam pour modifier les params d'extraction et d'ajout des grains
*/

/*
Fonctionnement du buffer tournant :
Le pointeur de lecteur est un echantillon avant le pointeur d'�criture
*/

#define GRAIN_CARD_BUFFER_LENGTH 0.1f //On prend 100ms de buffer sur la carte
#define GRAIN_SYNTHE_BUFFER_LENGTH 1.0f //On prend 1s de buffer en memoire

#include "sound.h"


/**
  * Utilise plusieurs buffers :
  * un buffer qui contient le fichier de base
  * un buffer dans lequel on fait la synth�se : on y ajoute les grains les un a la suite des autres
  *   en tenant compte de params de synth�se. C'est un buffer tournant.
  * un buffer temporaire qui permet de cr�er
  */
class SoundGrain : public Sound
{
private:

	/**
	  * Histoire de vous inspirer, je vous ai laiss� les attrbuts qu'utilise la classe une fois cod�e.
	  * vous pouvez les modifier a loisir.
	  */

	//Datas du fichier de base
	void * _BaseBuffer;
	float _DureeBase;
	uint8 * _PtRdFinBaseBuffer;
	ALenum _Format;
	ALsizei _Size;
	ALfloat _Frequency;
	int _NbPistes;
	int _NbBits;
	int _TailleEchantillon;

	//Buffers pour queuing openal
	ALuint _Buffers[3]; ///<id des buffers
	ALuint _PlayingBuffer; ///< indice du buffer en cours de lecture par la carte
	float _DureeBuffer; ///< Duree d'un buffer openal
	int _TailleBuffer; ///< Taille d'un buffer openal
	void * _DatasToLoad; ///< buffer temporaire pour balancer les datas � openal

	//Buffer pour faire la synth�se (buffer tournant)
	//necessaire car overlap des grains et sinon artefacts entre buffers
	float _DureeBufferSynthe; //Duree du buffer de synthe
	void * _DatasSynthe; 
	uint8 * _PtRdSynthe; ///< Pointeur de lecture du buffer de syntthe vers les buffers openal
	uint8 * _PtWrSynthe; ///< Pointeur d'ecriture dans le buffer de synthe
	uint8 * _PtFinBufferSynthe; ///< Pointeur sur la fin du buffer de synthe
	int _TailleBufferSynthe; ///< taille du buffer de synth�

	//Param�tres de la generation de grains
	float _PosGrainf; ///< Position de prise du grain entre 0 et 1
	float _DureeGrain; ///< Dur�e du grain en secondes 
	int _TailleGrain; ///< Taille du grain en octets
	int _PosGraini; ///< Position du grain en octets
	int _RandomWidth; //Taille du random quand on chope un grain
	int _SizeOverlap; //Taille d'overlapp entre les grains

	//Pour la lecture / ecriture des grains. 
	//On doit l'avoir au niveau de l'objet pour garder les params entre deux buffers successifs
	uint8 * _PtRdGrain;
	uint8 * _PtRdFinGrain;
	int _TailleLastGrain;


private :

	/**
	  * EXERCICE 2
	  * Cette m�thode vous est donn�e. Elle se contente de charger le buffer principal � la suite des buffers openal.
	  */
	void addBaseDataToQueue(void)
	{
		alBufferData(_Buffers[0],_Format,_BaseBuffer,_Size,(ALsizei)_Frequency);

		ALuint error = alGetError ();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			fprintf (stderr, "%s\n", alGetString (error));
		}

		alSourceQueueBuffers(_Source,1,_Buffers);
		error = alGetError ();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			fprintf (stderr, "%s\n", alGetString (error));
		}
	}

	/**
	  * EXERCICE 2 :
	  * TODO : inspirez vous de la m�thode addBaseDataToQueue() et g�n�rez un buffer qui contient un signal sinusoidal,
	  * puis ajoutez le a openal
	  */
	void addSinusoidalDataToQueue(float freqSignal)
	{
		Log::log(Log::ENGINE_ERROR, "addSinusoidalDataToQueue pas encore codee !!");
	}

	/**
	  * EXERCICE 3 : Phase de synth�se
	  * TODO : Cette m�thode va lire le buffer qui contient le fichier de base pour en extraire de grains
	  * qu'elle va copier au fur et � mesure dans le buffer de synth�se
	  */
	void fillBufferWithGrains(void)
	{			
		Log::log(Log::ENGINE_ERROR, "fillBufferWithGrains pas encore codee !!");
	}

	/**
	  * EXERCICE 3 : Phase de copie
	  * TODO : Cette m�thode rempli un buffer m�moire avec une partie du buffer de synth�se.
	  * elle copie ensuite ces donn�es vers un buffer opanal et l'ajoute � la suite de a liste de buffers.
	  * on utilise 3 buffers openal pour g�rer la file d'attente.
	  */
	void addBufferToQueue(int numBuffer)
	{
		Log::log(Log::ENGINE_ERROR, "addBufferToQueue pas encore codee !!");
	}

	/**
	  * EXERCICE 3 : 
	  * Cette m�thode vous est donn�e, elle retire un buffer de la liste des buffers openal
	  */
	void removeBufferFromQueue(int numBuffer)
	{
		alSourceUnqueueBuffers(_Source,1,&(_Buffers[numBuffer]));
		ALuint error = alGetError ();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			fprintf (stderr, "%s\n", alGetString (error));
		}
	}

	/**
	  * EXERCICE 3 : 
	  * Cette m�thode vous est donn�e, elle r�cup�re le nombre de buffers deja parcourus par openal
	  */
	int getNbBuffersProcessed(void)
	{
		ALint  value;
		alGetSourcei(_Source,AL_BUFFERS_PROCESSED,&value);
		ALuint error = alGetError ();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			fprintf (stderr, "%s\n", alGetString (error));
		}
		return value;
	}

	/**
	  * EXERCICE 3 : 
	  * Cette m�thode vous est donn�e, elle r�cup�re le nombre de buffers dans la liste de buffers en attente
	  */
	int getNbBuffersQueued(void)
	{
		ALint  value;
		alGetSourcei(_Source,AL_BUFFERS_QUEUED,&value);
		ALuint error = alGetError ();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			fprintf (stderr, "%s\n", alGetString (error));
		}
		return value;
	}


public:

	SoundGrain() : Sound()
	{
		_BaseBuffer = NULL;
		_PlayingBuffer = 0;
		_DureeBuffer = GRAIN_CARD_BUFFER_LENGTH; 
		_DureeBufferSynthe = GRAIN_SYNTHE_BUFFER_LENGTH;
		_PtRdGrain = NULL;
		_DureeBase = 0.0f;

		alGenBuffers(3,_Buffers);
		ALuint error = alGetError ();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			fprintf (stderr, "%s\n", alGetString (error));
		}
	}

	float getDureeBase(void)
	{
		return _DureeBase;
	}

	/**
	  * EXERCICE 1 : 
	  * TODO : Cette m�thode charge le fichier de base dans le buffer de base. Utiliser alut, et afficher
	  * sur la console le format du fichier charg� (voir �nonc�)
	  */
	void loadBaseFile(char * filename)
	{
		Log::log(Log::ENGINE_ERROR, "loadBaseFile pas encore codee !!");

		//On set des params par defaut au cas ou
		setGrainParam(0.5f,0.2f,0.2f,0.05f);
	}

	/**
	  * EXERCICE 3 : 
	  * TODO : modifiez vos param�tres de synth�se quand cette fonction est appel�e
	  */
	void setGrainParam(float posGrain, float dureeGrain, float randomPos, float partOverlap)
	{
		Log::log(Log::ENGINE_ERROR, "setGrainParam pas encore codee !!");
	}

	

	void play(void) 
	{
		
		while(getNbBuffersQueued() < 2)
		{
			_PlayingBuffer = (_PlayingBuffer + 1)%3;
			removeBufferFromQueue(_PlayingBuffer);
			fillBufferWithGrains();
			addBufferToQueue(_PlayingBuffer);
		}

		Sound::play();
	}


	void update(float elapsed)
	{
		Sound::update(elapsed);

		if(getNbBuffersProcessed() > 0)
		{
			_PlayingBuffer = (_PlayingBuffer + 1)%3;
			removeBufferFromQueue(_PlayingBuffer);
			fillBufferWithGrains();
			addBufferToQueue(_PlayingBuffer);
		}
	}
};

#endif
