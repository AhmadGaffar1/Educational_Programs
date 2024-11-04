///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// General Guides for Coding Style
/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// General Guides for Coding Style (end)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// TransformerMode Guides
/* Learnable Variables  ( Embedding_ )
 * 01- Embedding_Word
 * 02- Embedding_PositionOfWordInSentence
 * 03- Embedding_PositionOfSentenceInDocument
 *
 *
 *
 * Learnable Parameters ( Weights_ )
 * 01- Weights_LinearLayer
 * 02- Weights_SumDifferentVisions
 * 03- Weights_FullyConnected
 * 04- Weights_Gamma    , Weights_Beta    , Weights_EpsScalar
 * 05- Weights_Gamma_FC , Weights_Beta_FC , Weights_EpsScalar_FC
 *
 *
 *
 * Not Learnable ( Values_ )
 *
 *
 *
 * Not Learnable ( Errors )
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// TransformerMode Guides (end)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// libraries
#include"io.h"    // Syscall Library ( Handle Files )
#include"stdio.h" // C library ( Handle Console )
#include"vector"  // vector < > ( Reading Sentences , Storing Sentences )
#include"math.h"  // rand() , pow() , abs()
#include"thread"  // ( Handle Multi Threading )
#include"time.h"  // Calculate Time ( Use It When We Want Measure and Compare Time Between Some Processes )
using namespace std;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// libraries (end)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// #define  &  global features
#define maxInput 100000000 //  ( 10^8 )



#define maxWords 65536     //  ( 2^16 ) = ( 1<<16 ) = ( 65536 )

#define maxWordLength 32



#define SentenceLength 256

#define DocumentLengthBySentence 2

// ( + DocumentLengthBySentence ) --> ( 1 ) For Special Tokens That Added Before Each Sentence
#define DocumentLengthByWord ( DocumentLengthBySentence * SentenceLength + DocumentLengthBySentence )



#define PAD 256  // Not Use Padding
#define MASK 257
#define CLS 258
#define SEP 259
#define UNK 260
#define TIT 261



#define BlocksOfEncoders 4

// First Encoder [ 0 ] Use ( FeaturesIn == FeaturesOut of Previous Block ) and ( FeaturesOut == FeaturesOut Current Block )
const int EncodersNumber_Block [ BlocksOfEncoders ] = {1 + 64 , 1 + 16 , 1 + 4 , 1 + 1 };



#define EncoderLinearLayers 5



#define FeaturesOut_Embedding_Word 1024

#define FeaturesOut_Embedding_PositionOfWordInSentence 256

#define FeaturesOut_Embedding_PositionOfSentenceInDocument 64

#define FeaturesOut_Embedding_Bias 16

// Concatenate
#define FeaturesOut_Embedding_Document ( FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence + FeaturesOut_Embedding_PositionOfSentenceInDocument + FeaturesOut_Embedding_Bias )

const int FeaturesOut_Block [ BlocksOfEncoders ] = {1024,256,64,16 };



#define FullyConnectedLayers 5

// Neurons [ LastSentenceIndexInDocument Layer ] --> ( 16 )  ,  all this neurons doing same thing ( predict same value ) for increase error effect
const int Neurons [ FullyConnectedLayers ] = {4096 ,1024 ,256 ,64 ,16 };

#define NumberOfWeights_FullyConnectedLayer_0 ( DocumentLengthByWord * FeaturesOut_Block [ BlocksOfEncoders - 1 ] )


#define Linear 0
#define LeakyReLU 1
#define Tanh 2
#define Softmax 3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  #define  &  global features (end)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// classes
class PreparedData
{
public:
    void Prepare()
    {
        // English Words Used ///////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < maxInput; ++i) Input[i] = '\0';
        for (int i = 0; i < maxWords; ++i) for (int j = 0; j < maxWordLength; ++j) Dictionary[maxWords][maxWordLength] = '\0';
        for (int i = 0; i < maxWords; ++i) Freq[i] = 0;
        NumberOfWords = 0;
        Diff_a_A = 'a' - 'A';


        // Sentences Stored /////////////////////////////////////////////////////////////////////////////////////////////
        // All This Features are ( Vector ) , Vector Not Require Initialized Value


        // Temporary Variables for Reading Text /////////////////////////////////////////////////////////////////////////
        LastTitle= LastSentence=0;


        // Temporary Variables for Search ( Index of Word ) for Store Sentence by Indices Of her Words in ( SentencesByIndices ) //////////
        Start= Last= Mid=0;
    }

    void ReadingVocabulary(const char* file, char* store, char sep = '\n')
    {
        for (int i = 0; i < maxInput; ++i) store[i] = '\0';

        int fd;
        fd = open(file, 0x0000); // 0x0100 == O_CREAT // 0x0001 == O_WRONLY // 0x0000 == O_RDONLY    ---->>    fd = open( "words.txt", 0x0000 | 0x0001 | 0x0100 );
        read(fd, store, maxInput);
        close(fd);

        for (int i = 0; i < maxInput; ++i)
        {
            if (store[i] >= 'A' && store[i] <= 'Z')
            {
                store[i] += Diff_a_A;
            }
        }
    }

    void StoringVocabulary(char* store)
    {
        for (int i = 0; i < 256; ++i) Dictionary[i][0] = i, Freq[i] = 1;                                                                                           // ASCII Code
        Dictionary[256][0] = '['; Dictionary[256][1] = 'P'; Dictionary[256][2] = 'A'; Dictionary[256][3] = 'D'; Dictionary[256][4] = ']';                          // [PAD]
        Dictionary[257][0] = '['; Dictionary[257][1] = 'M'; Dictionary[257][2] = 'A'; Dictionary[257][3] = 'S'; Dictionary[257][4] = 'K'; Dictionary[1][5] = ']'; // [MASK]
        Dictionary[258][0] = '['; Dictionary[258][1] = 'C'; Dictionary[258][2] = 'L'; Dictionary[258][3] = 'S'; Dictionary[258][4] = ']';                          // [CLS]
        Dictionary[259][0] = '['; Dictionary[259][1] = 'S'; Dictionary[259][2] = 'E'; Dictionary[259][3] = 'P'; Dictionary[259][4] = ']';                          // [SEP]
        Dictionary[260][0] = '['; Dictionary[260][1] = 'U'; Dictionary[260][2] = 'N'; Dictionary[260][3] = 'K'; Dictionary[260][4] = ']';                          // [UNK]
        Dictionary[261][0] = '@'; Dictionary[261][1] = '@'; Dictionary[261][2] = '@'; Dictionary[261][3] = '@'; Dictionary[261][4] = '@';                          // @@@@@ for titles of articles

        Freq[256] = Freq[257] = Freq[258] = Freq[259] = Freq[260] = Freq[261] = 1;

        NumberOfWords=261;



        // FirstSentenceIndexInDocument from Dictionary[ 262 ] ,    Dictionary[ 0 : 255 ] for ASCII Code    &    Dictionary[ 256 : 261 ] for special tokens

        int i = 0, j = 0, w = 262;

        for (i = 0; store[i] != '\0'; ++i)
        {
            if (store[i] != '\n')
            {
                Dictionary[w][j++] = store[i];
            }

            else
            {
                ++w;
                j = 0;
                ++NumberOfWords;
            }
        }

        ++NumberOfWords;
    }

    inline void GetFileNumber(char* str, int num)
    {
        str[0] = '0' + (num / 100);
        str[1] = '0' + ((num % 100) / 10);
        str[2] = '0' + ((num % 100) % 10);
    }

    void ReadingSentences(const char* file, char* store, char sep = '\n')
    {
        for (int i = 0; i < maxInput; ++i) store[i] = '\0';

        int fd;
        fd = open(file, 0x0000); // 0x0100 == O_CREAT // 0x0001 == O_WRONLY // 0x0000 == O_RDONLY    ---->>    fd = open( "words.txt", 0x0000 | 0x0001 | 0x0100 );
        read(fd, store, maxInput);
        close(fd);
    }

    void StoringSentences(char* store)
    {
        for (int i = 0; store[i] != '\0'; ++i)
        {
            if
            (
                store[i + 0] == '\"' &&
                store[i + 1] == 't'  &&
                store[i + 2] == 'e'  &&
                store[i + 3] == 'x'  &&
                store[i + 4] == 't'  &&
                store[i + 5] == '\"' &&
                store[i + 6] == ':'
            )
            {
                i = i + 9;
                Article(store, i);
            }



            else if
            (
                store[i + 0] == '\"' &&
                store[i + 1] == 't'  &&
                store[i + 2] == 'i'  &&
                store[i + 3] == 't'  &&
                store[i + 4] == 'l'  &&
                store[i + 5] == 'e'  &&
                store[i + 6] == '\"' &&
                store[i + 7] == ':'
            )
            {
                i = i + 10;
                Title(store, i);
                LastTitle= (SentencesByWords.size()) - 1;
            }
        }
    }

    void Title(char* store, int& I)
    {
        Titles.push_back(SentencesByWords.size());

        SameWord.push_back('@');
        SameWord.push_back('@');
        SameWord.push_back('@');
        SameWord.push_back('@');
        SameWord.push_back('@');

        SameSentence.push_back(SameWord);

        SameWord.clear();

        for (; ( ! ( store[I] == '\"' && store[I + 1] == '\n' ) ) && store[I]!='\n' ; ++I) // same text
        {
            if ( ( ! ( store[I] == '.' && store[I + 1] == ' ' ) ) && ( ! ( store[I] == ';' && store[I + 1] == ' ' ) ) && ( ! ( store[I] == '?' && store[I + 1] == ' ' ) ) ) // same sentence
            {
                if (Alphabet(store[I])) SameWord.push_back(store[I]); // same Embedding_Word

                else
                {
                    SameSentence.push_back(SameWord);

                    SameWord.clear();

                    if (store[I] != ' ')
                    {
                        SameWord.push_back(store[I]);

                        SameSentence.push_back(SameWord);

                        SameWord.clear();
                    }
                }
            }
        }

        if(SameWord.size())     SameSentence.push_back(SameWord);

        if(SameSentence.size()) SentencesByWords.push_back(SameSentence);

        SameWord.clear();

        SameSentence.clear();
    }

    void Push_sameSentence()
    {
        LastSentence = (SentencesByWords.size()) - 1;

        if(LastSentence >= 0 && LastTitle < LastSentence)
        {
            if(SentencesByWords[LastSentence].size() + SameSentence.size() < SentenceLength)
            {
                for(int i=0; i < SameSentence.size(); ++i)
                {
                    SentencesByWords[LastSentence].push_back(SameSentence[i]);
                }
            }

            else
            {
                SentencesByWords.push_back(SameSentence);
            }
        }

        else
        {
            SentencesByWords.push_back(SameSentence);
        }
    }

    void Article(char* store, int& I)
    {
        for (; ( ! ( store[I] == '\"' && store[I+1]==',' && store[I+2]=='\n' ) ) && store[I]!='\n' ; ++I) // same text
        {
            if ( ( ! ( store[I] == '.' && store[I+1] == ' ' ) ) && ( ! ( store[I] == ';' && store[I+1] == ' ' ) ) && ( ! ( store[I] == '?' && store[I+1] == ' ' ) ) ) // same sentence
            {
                if( Alphabet(store[I] ) ) // same Embedding_Word
                {
                    SameWord.push_back(store[I]);
                }

                else
                {
                    SameSentence.push_back(SameWord);

                    SameWord.clear();

                    if(store[I]!=' ')
                    {
                        SameWord.push_back(store[I]);

                        SameSentence.push_back(SameWord);

                        SameWord.clear();
                    }
                }
            }

            else
            {
                if(SameWord.size())
                {
                    SameSentence.push_back(SameWord);

                    SameWord.clear();
                }



                if (store[I] == '.')        SameWord.push_back('.');

                else if (store[I] == ';')   SameWord.push_back(';');

                else if (store[I] == '?')   SameWord.push_back('?');



                SameSentence.push_back(SameWord);

                SameWord.clear();



                Push_sameSentence();

                SameSentence.clear();
            }
        }


        if(SameWord.size())     SameSentence.push_back(SameWord);
        if(SameSentence.size()) Push_sameSentence();
        SameWord.clear();
        SameSentence.clear();
    }

    bool Alphabet(const char& C)
    {
        if((C>='a' && C<='z') || C==22 || C==39 || C==45 || C==95 || C==149 || C==196) return 1;
        return 0;
    }

    void Normalize(char* store)
    {
        int diff_a_A = int('a') - int('A');
        for (int i = 0; i < maxInput; ++i)
        {
            // if ( i > 3302 && i < 3312) printf( "[%i]:\t%c    ,    %i\n", store[i], store[i] ); // debug

            if (store[i] >= 'A' && store[i] <= 'Z') store[i] += diff_a_A;  // 26

            else
            {
                // this compiler store character by ( ASCII Number - 256 ) !!!          // ASCII [ 8 ] for delete this character and backspace
                switch (store[i])
                {
                    case (130 - 256): { store[i - 1] = 8;    store[i] = 'e';    break; }
                    case (131 - 256): { store[i - 1] = 8;    store[i] = 'a';    break; }
                    case (133 - 256): { store[i - 1] = 8;    store[i] = 'a';    break; }
                    case (135 - 256): { store[i - 1] = 8;    store[i] = 'c';    break; }
                    case (136 - 256): { store[i - 1] = 8;    store[i] = 'e';    break; }
                    case (137 - 256): { store[i - 1] = 8;    store[i] = 'e';    break; }
                    case (138 - 256): { store[i - 1] = 8;    store[i] = 'e';    break; }
                    case (139 - 256): { store[i - 1] = 8;    store[i] = 'i';    break; }
                    case (140 - 256): { store[i - 1] = 8;    store[i] = 'i';    break; }
                    case (147 - 256): { store[i - 1] = 8;    store[i] = 'o';    break; }
                    case (150 - 256): { store[i - 1] = 8;    store[i] = 'u';    break; }
                    case (151 - 256): { store[i - 1] = 8;    store[i] = 'u';    break; }
                }
            }

            // if (i > 3000 && i < 3500) printf("%c", store[i]);// debug
        }

        // int z; // debug
        // printf("Z : "); // debug
        // scanf("%i",&z); // debug
        // printf("%s\n",store); // debug
    }

    void IndexingWordsInSentence()
    {
        for (int i = 0; i < SentencesByWords.size(); ++i)
        {
            vector<unsigned short> vec;
            SentencesByIndices.push_back(vec);
        }

        for (int i = 0; i < SentencesByWords.size(); ++i)
        {
            for(int j = 0; j < SentencesByWords[i].size(); ++j)
            {
                SentencesByIndices[i].push_back( IndexingWord ( SentencesByWords [i] [j] ) );
            }
        }
    }

    int IndexingWord(vector<char> word)
    {
        Start = 0; Last = NumberOfWords; Mid = (Start + Last ) / 2; int c = 0;
        while(Start <= Last)
        {
            for(c = 0; c < word.size() && Dictionary[Mid][c] != '\0'; ++c)
            {
                if(word[c] == Dictionary [Mid] [c] ) continue;

                else if(word[c] < Dictionary[Mid][c])
                {
                    Last = Mid - 1;
                    Mid = (Start + Last ) / 2;
                    break;
                }

                else if(word[c] > Dictionary[Mid][c])
                {
                    Start = Mid + 1;
                    Mid = (Start + Last ) / 2;
                    break;
                }
            }

            if(c==word.size() && Dictionary[Mid][c] == '\0') break;

            else if(c==word.size() && Dictionary[Mid][c] != '\0')
            {
                Last = Mid - 1;
                Mid = (Start + Last ) / 2;
            }

            else if(c<word.size() && Dictionary[Mid][c] == '\0')
            {
                Start = Mid + 1;
                Mid = (Start + Last ) / 2;
            }
        }



        if(Start > Last) return UNK;

        else           return Mid;
    }

    void PrintVocabulary()
    {
        printf("----------------------------------------------------------------\n");

        for (int i = 0; i < NumberOfWords; ++i)
        {
            printf("Word[%i] :: %s\n", i, Dictionary[i]);
        }

        printf("Number Of Dictionary :: %i\n", NumberOfWords);

        printf("----------------------------------------------------------------\n");
    }

    void PrintSentencesByWords()
    {
        for(int i=0; i < SentencesByWords.size() && i < 100; ++i)
        {
            printf("Sentence[%i]:\n",i);

            for(int j=0; j < SentencesByWords[i].size(); ++j)
            {
                for(int k=0; k < SentencesByWords[i][j].size(); ++k)
                {
                    printf("%c", SentencesByWords[i][j][k]);
                }

                printf(" ");
            }

            printf("\n----------------------------------------------------------------------------\n");
        }
    }

    void PrintSentencesByIndices()
    {
        for(int i=0; i < SentencesByIndices.size() && i < 100; ++i)
        {
            printf("Sentence[%i]:\n",i);

            for(int j=0;j<SentencesByIndices[i].size();++j)
            {
                printf("%i ",SentencesByIndices[i][j]);
            }

            printf("\n----------------------------------------------------------------------------\n");
        }
    }

public:
    // English Words Used ///////////////////////////////////////////////////////////////////////////////////////////////
    char Input[maxInput];
    char Dictionary[maxWords][maxWordLength];
    unsigned int Freq[maxWords], NumberOfWords, Diff_a_A;



    // Sentences Stored /////////////////////////////////////////////////////////////////////////////////////////////////
    vector<vector<unsigned short>> SentencesByIndices;
    vector<vector<vector<char>>> SentencesByWords;
    vector<unsigned int> Titles; // Stored Indices of ( Sentence it's a Title )



    // Temporary Variables for Reading Text /////////////////////////////////////////////////////////////////////////////
    vector<vector<char>> SameSentence;
    vector<char> SameWord;
    unsigned int LastTitle,LastSentence;



    // Temporary Variables for Search ( Index of Word ) for Store Sentence by Indices Of her Words in ( SentencesByIndices ) //////////
    unsigned int Start,Last,Mid;
};

class TransformerModel
{
public:
    void PrepareFunction_00()
    {
        PrepareFunction_01_DataVariables();

        PrepareFunction_02_InitializingWeightsFactors();

        PrepareFunction_03_DocumentVariables();

        PrepareFunction_04_EmbeddingLayer_Variables();

        PrepareFunction_05_TransformerModel_00();

        PrepareFunction_06_ClassificationLayer_00();

        PrepareFunction_07_AdjustmentVariables();

        PrepareFunction_08_MultiThreadingVariables();

        PrepareFunction_09_MomentumRate_LearningRate();

        PrepareFunction_10_SpecialHyperParameters();

        PrepareFunction_11_TemporaryVariables();
    }

    void PrepareFunction_01_DataVariables()
    {
        NumberOfTrainingFile = 605;
        NumberOFTestingData = 0;
        Correct = 0;



        // this block of code responsible for reading and storing Dictionary;

        Data.Prepare();

        Data.ReadingVocabulary("63666 English words in Lowercase .txt", Data.Input);

        Data.StoringVocabulary(Data.Input);

        // Data.PrintVocabulary(); // test



        // this block of code responsible for reading and storing SentencesByWords;

        char file[9] = { '0','0', '0', '.', 'j', 's', 'o', 'n', '\0' };

        for (int i = 1; i < NumberOfTrainingFile; ++i)
        {
            Data.GetFileNumber(file, i);

            // printf("file : %s\n",file); // test

            Data.ReadingSentences(file, Data.Input);

            Data.Normalize(Data.Input);

            Data.StoringSentences(Data.Input);
        }

        // Data.PrintSentencesByWords(); // test



        // this block of code responsible for reading and storing SentencesByIndices;

        Data.IndexingWordsInSentence();

        //Data.PrintSentencesByIndices(); // test
    }

    void PrepareFunction_02_InitializingWeightsFactors()
    {
        // Factor == max random value can generate by rand() function  *  number of neurons in previous layer ^ 0.5 /////

        Factor_Embedding_Word     = (double) (1 << 15) * pow(FeaturesOut_Embedding_Word, 0.5);

        Factor_Embedding_PositionOfWordInSentence = (double) (1 << 15) * pow(FeaturesOut_Embedding_PositionOfWordInSentence, 0.5);

        Factor_Embedding_PositionOfSentenceInDocument = (double) (1 << 15) * pow(FeaturesOut_Embedding_PositionOfSentenceInDocument, 0.5);

        Factor_Embedding_Sequence = (double) (1 << 15) * pow(FeaturesOut_Embedding_Document, 0.5);



        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Factor_Weights_LinearLayer_Block [ BlockIndex ] = (double) (1 << 15) * pow(FeaturesOut_Block [ BlockIndex ], 0.5);
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Factor_Weights_SumDifferentVisionsOfValues_Block [ BlockIndex ] = (double) (1 << 15) * pow(3, 0.5);
        }



        for(int Layer = 1; Layer < FullyConnectedLayers; ++Layer )
        {
            Factor_Weights_FullyConnected[ Layer ] = (double) (1 << 15) * pow(Neurons[ Layer - 1 ], 0.5);
        }

        Factor_Weights_FullyConnected[ 0 ] = (double) (1 << 15) * pow(NumberOfWeights_FullyConnectedLayer_0, 0.5);
    }

    void PrepareFunction_03_DocumentVariables()
    {
        // Embedding_Document ///////////////////////////////////////////////////////////////////////////////////////////

        for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
        {
            IndicesWordsInDictionary_Document [ WordIndexInDocument ] = 0;

            IndicesWordsInSentence_Document [ WordIndexInDocument ] = 0;

            IndicesSentenceInDocument_Document [ WordIndexInDocument ] = 0;

            Masks_Document [ WordIndexInDocument ] = 0;
        }

        for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
        {
            for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Embedding_Document; ++FeaturesOut)
            {
                Values_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                Errors_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut ] = 0;
            }
        }

        for(int Sentence = 0; Sentence < DocumentLengthBySentence; ++Sentence)
        {
            MaskingNumberInSentence [ Sentence ] = 0;
        }



        NumberOfWordsInCurrentDocument = 0;

        LengthOfLargestSentenceInCurrentDocument = 0;

        FirstSentenceIndexInDocument = 0;

        LastSentenceIndexInDocument = 0;



        FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 0 ] = 0;

        FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 1 ] = FeaturesOut_Embedding_Word;

        FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 2 ] = FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence;

        FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 3 ] = FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence + FeaturesOut_Embedding_PositionOfSentenceInDocument;

    }

    void PrepareFunction_04_EmbeddingLayer_Variables()
    {
        // Embedding_Word ///////////////////////////////////////////////////////////////////////////////////////////////

        // Embedding_Word ( Learnable Variables ) ( Random Initialization )
        for (int WordIndexInDictionary = 0; WordIndexInDictionary < maxWords; ++WordIndexInDictionary)
        {
            for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Embedding_Word; ++FeaturesOut)
            {
                Rand0 = rand();

                Embedding_Word [ WordIndexInDictionary ] [ FeaturesOut ] = Rand0 / Factor_Embedding_Word;

                if ( Rand0 % 3 == 0 )
                {
                    Embedding_Word [ WordIndexInDictionary ] [ FeaturesOut ] =
                    - Embedding_Word [ WordIndexInDictionary ] [ FeaturesOut ];
                }
            }
        }

        // Embedding_Word_Delta ( Zero Initialization )
        for (int WordIndexInDictionary = 0; WordIndexInDictionary < maxWords; ++WordIndexInDictionary)
        {
            for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Embedding_Word; ++FeaturesOut)
            {
                Embedding_Word_Delta [ WordIndexInDictionary ] [ FeaturesOut ] = 0;
            }
        }





        // Embedding_PositionOfWordInSentence ///////////////////////////////////////////////////////////////////////////

        // Embedding_PositionOfWordInSentence ( Learnable Variables ) ( Random Initialization )
        for (int WordIndexInSentence = 0; WordIndexInSentence < SentenceLength + 1; ++WordIndexInSentence)
        {
            for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Embedding_PositionOfWordInSentence; ++FeaturesOut)
            {
                Rand0 = rand();

                Embedding_PositionOfWordInSentence [ WordIndexInSentence ] [ FeaturesOut ] = Rand0 / Factor_Embedding_PositionOfWordInSentence;

                if ( Rand0 % 3 == 0 )
                {
                    Embedding_PositionOfWordInSentence [ WordIndexInSentence ] [ FeaturesOut ] =
                    - Embedding_PositionOfWordInSentence [ WordIndexInSentence ] [ FeaturesOut ];
                }
            }
        }

        // Embedding_PositionOfWordInSentence_Delta ( Zero Initialization )
        for (int WordIndexInSentence = 0; WordIndexInSentence < SentenceLength + 1; ++WordIndexInSentence)
        {
            for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Embedding_PositionOfWordInSentence; ++FeaturesOut)
            {
                Embedding_PositionOfWordInSentence_Delta [ WordIndexInSentence ] [ FeaturesOut ] = 0;
            }
        }





        // Embedding_PositionOfSentenceInDocument ///////////////////////////////////////////////////////////////////////

        // Embedding_PositionOfSentenceInDocument ( Learnable Variables ) ( Random Initialization )
        for (int SentenceIndexInDocument = 0 ; SentenceIndexInDocument < DocumentLengthBySentence ; ++SentenceIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Embedding_PositionOfSentenceInDocument ; ++FeaturesOut )
            {
                Rand0 = rand();

                Embedding_PositionOfSentenceInDocument [ SentenceIndexInDocument ] [ FeaturesOut ] = Rand0 / Factor_Embedding_PositionOfSentenceInDocument;

                if ( Rand0 % 3 == 0 )
                {
                    Embedding_PositionOfSentenceInDocument [ SentenceIndexInDocument ] [ FeaturesOut ] =
                    - Embedding_PositionOfSentenceInDocument [ SentenceIndexInDocument ] [ FeaturesOut ];
                }
            }
        }

        // Embedding_PositionOfSentenceInDocument_Delta ( Zero Initialization )
        for (int SentenceIndexInDocument = 0 ; SentenceIndexInDocument < DocumentLengthBySentence ; ++SentenceIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Embedding_PositionOfSentenceInDocument ; ++FeaturesOut )
            {
                Embedding_PositionOfSentenceInDocument_Delta [ SentenceIndexInDocument ] [ FeaturesOut ] = 0;
            }
        }





        // Embedding_PositionOfSentenceInDocument ///////////////////////////////////////////////////////////////////////

        for (int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Embedding_Bias ; ++FeaturesOut )
        {
            Embedding_Bias [ FeaturesOut ] = 1 ;
        }
    }

    void PrepareFunction_05_TransformerModel_00()
    {
        PrepareFunction_05_TransformerModel_01_WeightsLinearLayer();

        PrepareFunction_05_TransformerModel_02_ValuesInternalAttention_ErrorsInternalAttention();

        PrepareFunction_05_TransformerModel_03_ValuesQuires_ErrorsQuires();

        PrepareFunction_05_TransformerModel_04_ValuesKeys_ErrorsKeys();

        PrepareFunction_05_TransformerModel_05_ValuesEdges_ErrorsEdges();

        PrepareFunction_05_TransformerModel_06_ValuesExternalAttention0_ErrorsExternalAttention0();

        PrepareFunction_05_TransformerModel_07_ValuesExternalAttention1_ErrorsExternalAttention1();

        PrepareFunction_05_TransformerModel_08_WeightsSumDifferentVisions();

        PrepareFunction_05_TransformerModel_09_ValuesSumDifferentVisions_ErrorsSumDifferentVisions();

        PrepareFunction_05_TransformerModel_10_ValuesEncoder_ErrorsEncoder();
    }

    void PrepareFunction_05_TransformerModel_01_WeightsLinearLayer()
    {
        // Weights_LinearLayer ( Learnable Parameters ) /////////////////////////////////////////////////////////////////

        Weights_LinearLayer = new double **** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Weights_LinearLayer[BlockIndex] = new double *** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex=0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] = new double ** [ EncoderLinearLayers ];
            }
        }



        // Definition Pointers to Special Number of Locations ( Weights_LinearLayer [ BlocksOfEncoders ] [ EncodersNumber_Block [ BlockIndex ] ] [ EncoderLinearLayers ] [----] [----] )

        // if ( EncoderIndex > [ 0 ] )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIn)
                    {
                        Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ LinearLayer] [ FeaturesIn ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }
            }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex > [ 0 ] )
        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ BlockIndex - 1 ] ];
                }

        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [BlockIndex - 1 ]; ++FeaturesIn)
                    {
                        Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }

        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ BlockIndex ] ];
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block[BlockIndex]; ++FeaturesIn)
                    {
                        Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }

        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex == [ 0 ] )
        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Embedding_Document ];
                }


        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Embedding_Document; ++FeaturesIn)
                    {
                        Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ 0 ] ];
                    }
                }
        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ 0 ] ];
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn=0; FeaturesIn < FeaturesOut_Block[0]; ++FeaturesIn)
                    {
                        Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ 0 ] ];
                    }
                }



        // Initialize Weights of ( Weights_LinearLayer  == Random )

        // if ( EncoderIndex > [ 0 ] )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block[ BlockIndex ]; ++FeaturesOut)
                        {
                            Rand0 = rand();

                            Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = Rand0 / Factor_Weights_LinearLayer_Block [ BlockIndex ];

                            if (Rand0 % 3 == 0)
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] =
                                - Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ];
                        }
                    }
                }
            }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex > [ 0 ] )
        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block[ BlockIndex - 1 ]; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block[ BlockIndex ]; ++FeaturesOut)
                        {
                            Rand0 = rand();

                            Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = Rand0 / Factor_Weights_LinearLayer_Block [ BlockIndex - 1 ];

                            if (Rand0 % 3 == 0)
                                Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] =
                                - Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ];
                        }
                    }
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block[ BlockIndex ]; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block[ BlockIndex ]; ++FeaturesOut)
                        {
                            Rand0 = rand();

                            Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = Rand0 / Factor_Weights_LinearLayer_Block [ BlockIndex ];

                            if (Rand0 % 3 == 0)
                                Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] =
                                - Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ];
                        }
                    }
                }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex == [ 0 ] )
        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Embedding_Document; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ]; ++FeaturesOut)
                        {
                            Rand0 = rand();

                            Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = Rand0 / Factor_Embedding_Sequence;

                            if (Rand0 % 3 == 0)
                                Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] =
                                - Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ];
                        }
                    }
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ 0 ]; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ] ; ++FeaturesOut)
                        {
                            Rand0 = rand();

                            Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = Rand0 / Factor_Weights_LinearLayer_Block [ 0 ];

                            if (Rand0 % 3 == 0)
                                Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] =
                                - Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ];
                        }
                    }
                }





        // Weights_LinearLayer_Delta ////////////////////////////////////////////////////////////////////////////////////

        Weights_LinearLayer_Delta = new double **** [ BlocksOfEncoders ] ;

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Weights_LinearLayer_Delta [ BlockIndex ] = new double *** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex=0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] = new double ** [ EncoderLinearLayers ];
            }
        }



        // Definition Pointers to Special Number of Locations ( Weights_LinearLayer_Delta [ BlocksOfEncoders ] [ EncodersNumber_Block [ BlockIndex ] ] [ EncoderLinearLayers ] [----] [----] )

        // if ( EncoderIndex > [ 0 ] )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer=0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn=0; FeaturesIn < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIn)
                    {
                        Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }
            }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex > [ 0 ] )
        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    Weights_LinearLayer_Delta [ BlockIndex ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ BlockIndex - 1 ] ];
                }
        }
        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex - 1 ]; ++FeaturesIn)
                    {
                        Weights_LinearLayer_Delta [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer_Delta [ BlockIndex ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ BlockIndex ] ];
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block[ BlockIndex ]; ++FeaturesIn)
                    {
                        Weights_LinearLayer_Delta [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex == [ 0 ] )
        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    Weights_LinearLayer_Delta [ 0 ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Embedding_Document ];
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Embedding_Document; ++FeaturesIn)
                    {
                        Weights_LinearLayer_Delta [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ 0 ] ];
                    }
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer_Delta [ 0 ] [ 0 ] [ LinearLayer ] = new double * [ FeaturesOut_Block [ 0 ] ];
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ 0 ]; ++FeaturesIn)
                    {
                        Weights_LinearLayer_Delta [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] = new double [ FeaturesOut_Block [ 0 ] ];
                    }
                }



        // Initialize Values of ( Weights_LinearLayer_Delta  == 0 )

        // if ( EncoderIndex > [ 0 ] )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                        {
                            Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesIn ] [FeaturesOut ] = 0;
                        }
                    }
                }
            }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex > [ 0 ] )
        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [BlockIndex - 1 ]; ++FeaturesIn)
                    {
                        for (int FeaturesIOut = 0; FeaturesIOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIOut)
                        {
                            Weights_LinearLayer_Delta [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesIOut ] = 0;
                        }
                    }
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                        {
                            Weights_LinearLayer_Delta [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = 0;
                        }
                    }
                }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex == [ 0 ] )
        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Embedding_Document; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ]; ++FeaturesOut)
                        {
                            Weights_LinearLayer_Delta [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = 0;
                        }
                    }
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ 0 ]; ++FeaturesIn)
                    {
                        for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ]; ++FeaturesOut)
                        {
                            Weights_LinearLayer_Delta [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] = 0;
                        }
                    }
                }





        // Weights_LinearLayer_Sum //////////////////////////////////////////////////////////////////////////////////////

        Weights_LinearLayer_Sum = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Weights_LinearLayer_Sum [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] = new double * [ EncoderLinearLayers ];
            }
        }



        // Definition Pointers to Special Number of Locations ( Weights_LinearLayer_Sum [BlocksOfEncoders] [EncodersNumber_Block[b]] [EncoderLinearLayers] [---] )

        // if ( EncoderIndex > [ 0 ] )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer) {
                    Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex > [ 0 ] )
        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    Weights_LinearLayer_Sum [ BlockIndex ] [ 0 ] [ LinearLayer ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Weights_LinearLayer_Sum [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] = 0;
                    }
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer_Sum [ BlockIndex ] [ 0 ] [ LinearLayer ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Weights_LinearLayer_Sum [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] = 0;
                    }
                }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex == [ 0 ] )
        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ LinearLayer ] = new double [ FeaturesOut_Block [ 0 ] ];
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ]; ++FeaturesOut)
                    {
                        Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] = 0;
                    }
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    Weights_LinearLayer_Sum[ 0 ] [ 0 ] [ LinearLayer ] = new double [ FeaturesOut_Block [ 0 ] ];
                }

        /*********************************/
             /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ]; ++FeaturesOut)
                    {
                        Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] = 0;
                    }
                }



        // Collect Values of ( Weights_LinearLayer_Sum )

        // if ( EncoderIndex > [ 0 ] )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 1; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int LinearLayer = 0; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIn)
                        {
                            Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesOut ] +=

                                    abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] );
                        }
                    }
                }
            }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex > [ 0 ] )
        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex - 1 ]; ++FeaturesIn)
                        {
                            Weights_LinearLayer_Sum [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] +=

                                    abs(Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] );
                        }
                    }
                }
        }

        for(int BlockIndex = 1; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex ]; ++FeaturesIn)
                        {
                            Weights_LinearLayer_Sum [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] +=

                                    abs(Weights_LinearLayer [ BlockIndex ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ] );
                        }
                    }
                }
        }



        // if ( EncoderIndex == [ 0 ] && BlockIndex == [ 0 ] )
        /*********************************/
            /*********************************/
                for (int LinearLayer = 0; LinearLayer < 4; ++LinearLayer)
                {
                    for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ]; ++FeaturesOut)
                    {
                        for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Embedding_Document; ++FeaturesIn)
                        {
                            Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] +=

                                    abs(Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ]);
                        }
                    }
                }

        /*********************************/
            /*********************************/
                for (int LinearLayer = 4; LinearLayer < EncoderLinearLayers; ++LinearLayer)
                {
                    for (int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ 0 ]; ++FeaturesOut)
                    {
                        for(int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ 0 ]; ++FeaturesIn)
                        {
                            Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesOut ] +=

                                    abs(Weights_LinearLayer [ 0 ] [ 0 ] [ LinearLayer ] [ FeaturesIn ] [ FeaturesOut ]);
                        }
                    }
                }
    }

    void PrepareFunction_05_TransformerModel_02_ValuesInternalAttention_ErrorsInternalAttention()
    {
        // Values_InternalAttention /////////////////////////////////////////////////////////////////////////////////////
        Values_InternalAttention = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_InternalAttention [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }





        // Errors_InternalAttention ///////////////////////////////////////////////////////////////////////////////
        Errors_InternalAttention = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_InternalAttention [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }
    }

    void PrepareFunction_05_TransformerModel_03_ValuesQuires_ErrorsQuires()
    {
        // Values_Quires ////////////////////////////////////////////////////////////////////////////////////////////////
        Values_Quires = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_Quires [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_Quires [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }





        // Errors_Quires ////////////////////////////////////////////////////////////////////////////////////////////////
        Errors_Quires = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_Quires [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_Quires [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Errors_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Errors_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }
    }

    void PrepareFunction_05_TransformerModel_04_ValuesKeys_ErrorsKeys()
    {
        // Values_Keys //////////////////////////////////////////////////////////////////////////////////////////////////
        Values_Keys = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_Keys [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_Keys [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Values_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }





        // Errors_Keys //////////////////////////////////////////////////////////////////////////////////////////////////
        Errors_Keys = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_Keys [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_Keys [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Errors_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Errors_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }
    }

    void PrepareFunction_05_TransformerModel_05_ValuesEdges_ErrorsEdges()
    {
        // Values_Edges /////////////////////////////////////////////////////////////////////////////////////////////////
        Values_Edges = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_Edges [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_Edges [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord ; ++WordIndexInDocument)
                {
                    Values_Edges [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ DocumentLengthByWord ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int ActorWordIndexInDocument = 0; ActorWordIndexInDocument < DocumentLengthByWord; ++ActorWordIndexInDocument)
                {
                    for(int ReactedWordIndexInDocument = 0; ReactedWordIndexInDocument < DocumentLengthByWord ; ++ReactedWordIndexInDocument)
                    {
                        Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] = 0;
                    }
                }
            }
        }





        // Errors_Edges /////////////////////////////////////////////////////////////////////////////////////////////////
        Errors_Edges = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_Edges [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_Edges [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int ActorWordIndexInDocument = 0; ActorWordIndexInDocument < DocumentLengthByWord; ++ActorWordIndexInDocument)
                {
                    Errors_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] = new double [ DocumentLengthByWord ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int ActorWordIndexInDocument = 0; ActorWordIndexInDocument < DocumentLengthByWord; ++ActorWordIndexInDocument)
                {
                    for(int ReactedWordIndexInDocument = 0; ReactedWordIndexInDocument < DocumentLengthByWord ; ++ReactedWordIndexInDocument)
                    {
                        Errors_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] = 0;
                    }
                }
            }
        }

    }

    void PrepareFunction_05_TransformerModel_06_ValuesExternalAttention0_ErrorsExternalAttention0()
    {
        // Values_ExternalAttention0 ////////////////////////////////////////////////////////////////////////////////////
        Values_ExternalAttention0 = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_ExternalAttention0 [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }





        // Errors_ExternalAttention0 ////////////////////////////////////////////////////////////////////////////////////
        Errors_ExternalAttention0 = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_ExternalAttention0 [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }
    }

    void PrepareFunction_05_TransformerModel_07_ValuesExternalAttention1_ErrorsExternalAttention1()
    {
        // Values_ExternalAttention1 ////////////////////////////////////////////////////////////////////////////////////
        Values_ExternalAttention1 = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_ExternalAttention1 [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }





        // Errors_ExternalAttention1 ////////////////////////////////////////////////////////////////////////////////////
        Errors_ExternalAttention1 = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_ExternalAttention1 [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Errors_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Errors_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }
    }

    void PrepareFunction_05_TransformerModel_08_WeightsSumDifferentVisions()
    {
        // Weights_SumDifferentVisions ( Learnable Parameters ) /////////////////////////////////////////////////////////
        Weights_SumDifferentVisions = new double **** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Weights_SumDifferentVisions[BlockIndex] = new double ***[EncodersNumber_Block[BlockIndex]];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] = new double ** [ 3 ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int Vision = 0; Vision < 3; ++Vision)
                {
                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ Vision ] = new double * [ DocumentLengthByWord ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for (int Vision = 0; Vision < 3; ++Vision)
                {
                    for (int Word = 0; Word < DocumentLengthByWord; ++Word)
                    {
                        Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ Vision ] [ Word ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }
            }
        }

        // Initialize Weights of ( Weights_SumDifferentVisions == Random )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int Vision = 0; Vision < 3; ++Vision)
                {
                    for(int Word = 0; Word < DocumentLengthByWord; ++Word)
                    {
                        for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                        {
                            Rand0 = rand();

                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ Vision ] [ Word ] [ FeaturesOut ] = Rand0 / Factor_Weights_SumDifferentVisionsOfValues_Block [ BlockIndex ];

                            if (Rand0 % 3 == 0)
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ Vision ] [ Word ] [ FeaturesOut ] =
                                - Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ Vision ] [ Word ] [ FeaturesOut ];
                        }
                    }
                }
            }
        }





        // Weights_SumDifferentVisions_Delta ////////////////////////////////////////////////////////////////////
        Weights_SumDifferentVisions_Delta = new double **** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Weights_SumDifferentVisions_Delta [ BlockIndex ]=new double *** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] = new double ** [ 3 ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int Vision = 0; Vision < 3; ++Vision)
                {
                    Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ Vision ] = new double * [ DocumentLengthByWord ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int Vision = 0; Vision < 3; ++Vision)
                {
                    for(int Word = 0; Word < DocumentLengthByWord; ++Word)
                    {
                        Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ Vision ] [ Word ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                    }
                }
            }
        }

        // Initialize Values of ( Weights_SumDifferentVisions_Delta == 0 )
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int Vision = 0; Vision < 3; ++Vision)
                {
                    for(int Word = 0; Word < DocumentLengthByWord; ++Word)
                    {
                        for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                        {
                            Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ Vision ] [ Word ] [ FeaturesOut ] = 0;
                        }
                    }
                }
            }
        }
    }

    void PrepareFunction_05_TransformerModel_09_ValuesSumDifferentVisions_ErrorsSumDifferentVisions()
    {
        // Values_SumDifferentVisions ///////////////////////////////////////////////////////////////////////////////////
        Values_SumDifferentVisions = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_SumDifferentVisions [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }





        // Errors_SumDifferentVisions ///////////////////////////////////////////////////////////////////////////////////
        Errors_SumDifferentVisions = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_SumDifferentVisions [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }
    }

    void PrepareFunction_05_TransformerModel_10_ValuesEncoder_ErrorsEncoder()
    {
        // Values_Encoder ///////////////////////////////////////////////////////////////////////////////////////////////
        Values_Encoder = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Values_Encoder [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Values_Encoder [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }





        // Errors_Encoder ///////////////////////////////////////////////////////////////////////////////////////////////
        Errors_Encoder = new double *** [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            Errors_Encoder [ BlockIndex ] = new double ** [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                Errors_Encoder [ BlockIndex ] [ EncoderIndex ] = new double * [ DocumentLengthByWord ];
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] = new double [ FeaturesOut_Block [ BlockIndex ] ];
                }
            }
        }

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < DocumentLengthByWord; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }
    }

    void PrepareFunction_06_ClassificationLayer_00()
    {
        PrepareFunction_06_ClassificationLayer_01_WeightsFullyConnected();
        PrepareFunction_06_ClassificationLayer_02_ValuesFullyConnected_ErrorsFullyConnected();
    }

    void PrepareFunction_06_ClassificationLayer_01_WeightsFullyConnected()
    {
        // Weights_FullyConnected ( learnable parameters ) //////////////////////////////////////////////////////////////
        Weights_FullyConnected = new double ** [ FullyConnectedLayers ];

        for ( int LayerIndex = 0 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            Weights_FullyConnected [ LayerIndex ] = new double * [ Neurons [ LayerIndex ] ];
        }

        // if ( LayerIndex > [ 0 ] )
        for ( int LayerIndex = 1 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for ( int NeuronIndex = 0 ; NeuronIndex < Neurons [ LayerIndex ] ; ++NeuronIndex )
            {
                // ( + 1 ) --> Bias
                Weights_FullyConnected [ LayerIndex ] [ NeuronIndex ] = new double [ Neurons [ LayerIndex - 1 ] + 1 ];
            }
        }

        // if ( LayerIndex == [ 0 ] )
        /*********************************/
            for ( int NeuronIndex = 0 ; NeuronIndex < Neurons [ 0 ] ; ++NeuronIndex )
            {
                Weights_FullyConnected [ 0 ] [ NeuronIndex ] = new double [ NumberOfWeights_FullyConnectedLayer_0 ];
            }



        // Initialize Weights of ( Weights_FullyConnected == Random )

        // if ( LayerIndex > [ 0 ] )
        for ( int LayerIndex = 1; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ LayerIndex ] ; ++NeuronIndexOfCurrentLayer )
            {
                // ( + 1 ) --> Bias
                for ( int NeuronIndexOfPreviousLayer = 0 ; NeuronIndexOfPreviousLayer < Neurons [ LayerIndex - 1 ] + 1 ; ++NeuronIndexOfPreviousLayer )
                {
                    Rand0 = rand();

                    Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] = Rand0 / Factor_Weights_FullyConnected [ LayerIndex ];

                    if ( Rand0 % 3 == 0 )
                    {
                        Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] =

                                - Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ];
                    }
                }
            }
        }

        // if ( LayerIndex == [ 0 ] )
        /*********************************/
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ 0 ] ; ++NeuronIndexOfCurrentLayer )
            {
                for ( int NeuronIndexOfPreviousLayer = 0 ; NeuronIndexOfPreviousLayer < NumberOfWeights_FullyConnectedLayer_0 ; ++NeuronIndexOfPreviousLayer )
                {
                    Rand0 = rand();

                    Weights_FullyConnected [ 0 ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] = Rand0 / Factor_Weights_FullyConnected [ 0 ];

                    if ( Rand0 % 3 == 0 )
                    {
                        Weights_FullyConnected [ 0 ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] =

                                - Weights_FullyConnected [ 0 ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ];
                    }
                }
            }





        // Weights_FullyConnected_Delta /////////////////////////////////////////////////////////////////////////////////
        Weights_FullyConnected_Delta = new double ** [ FullyConnectedLayers ];

        for ( int LayerIndex = 0 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            Weights_FullyConnected_Delta [ LayerIndex ] = new double * [ Neurons [ LayerIndex ] ];
        }

        // if ( LayerIndex > [ 0 ] )
        for ( int LayerIndex = 1 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for ( int NeuronIndex = 0 ; NeuronIndex < Neurons [ LayerIndex ] ; ++NeuronIndex )
            {
                // ( + 1 ) --> Bias
                Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndex ] = new double [ Neurons[LayerIndex - 1 ] + 1 ];
            }
        }

        // if ( LayerIndex == [ 0 ] )
        /*********************************/
            for ( int NeuronIndex = 0 ; NeuronIndex < Neurons [ 0 ] ; ++NeuronIndex )
            {
                Weights_FullyConnected_Delta [ 0 ] [ NeuronIndex ] = new double [ NumberOfWeights_FullyConnectedLayer_0 ];
            }



        // Initialize Values of ( Weights_FullyConnected_Delta == 0 )

        // if ( LayerIndex > [ 0 ] )
        for ( int LayerIndex = 1 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ LayerIndex ] ; ++NeuronIndexOfCurrentLayer )
            {
                // ( + 1 ) --> Bias
                for ( int NeuronIndexOfPreviousLayer = 0 ; NeuronIndexOfPreviousLayer < Neurons [ LayerIndex - 1 ] + 1 ; ++NeuronIndexOfPreviousLayer )
                {
                    Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] = 0;
                }
            }
        }

        // if ( LayerIndex == [ 0 ] )
        /*********************************/
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ 0 ] ; ++NeuronIndexOfCurrentLayer )
            {
                for ( int NeuronIndexOfPreviousLayer = 0 ; NeuronIndexOfPreviousLayer < NumberOfWeights_FullyConnectedLayer_0 ; ++NeuronIndexOfPreviousLayer )
                {
                    Weights_FullyConnected_Delta [ 0 ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] = 0;
                }
            }





        // Weights_FullyConnected_Sum ///////////////////////////////////////////////////////////////////////////////////
        Weights_FullyConnected_Sum = new double * [ FullyConnectedLayers ];

        for ( int LayerIndex = 0 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            Weights_FullyConnected_Sum [ LayerIndex ] = new double [ Neurons [ LayerIndex ] ];
        }

        // if ( LayerIndex > [ 0 ] )
        for ( int LayerIndex = 1 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ LayerIndex ] ; ++NeuronIndexOfCurrentLayer )
            {
                Weights_FullyConnected_Sum [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] = 0;
            }
        }

        // if ( LayerIndex == [ 0 ] )
        /*********************************/
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ 0 ] ; ++NeuronIndexOfCurrentLayer )
            {
                Weights_FullyConnected_Sum [ 0 ] [ NeuronIndexOfCurrentLayer ] = 0;
            }



        // Collect Values of ( Weights_FullyConnected_Sum )

        // if ( LayerIndex > [ 0 ] )
        for ( int LayerIndex = 1 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ LayerIndex ] ; ++NeuronIndexOfCurrentLayer )
            {
                // ( + 1 ) --> Bias
                for( int NeuronIndexInPreviousLayer = 0 ; NeuronIndexInPreviousLayer < Neurons [ LayerIndex - 1 ] + 1 ; ++NeuronIndexInPreviousLayer )
                {
                    Weights_FullyConnected_Sum [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] +=

                            Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexInPreviousLayer ];
                }
            }
        }

        // if ( LayerIndex == [ 0 ] )
        /*********************************/
            for ( int NeuronIndexOfCurrentLayer = 0 ; NeuronIndexOfCurrentLayer < Neurons [ 0 ] ; ++NeuronIndexOfCurrentLayer )
            {
                for( int WordIndexInCurrentDocument = 0 ; WordIndexInCurrentDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInCurrentDocument )
                {
                    for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlocksOfEncoders - 1 ] ; ++FeaturesOut )
                    {
                        Weights_FullyConnected_Sum [ 0 ] [ NeuronIndexOfCurrentLayer ] +=

                                Weights_FullyConnected [ 0 ] [ NeuronIndexOfCurrentLayer ] [ WordIndexInCurrentDocument * FeaturesOut_Block [ BlocksOfEncoders - 1 ] + FeaturesOut ];
                    }
                }
            }

    }

    void PrepareFunction_06_ClassificationLayer_02_ValuesFullyConnected_ErrorsFullyConnected()
    {
        // Values_FullyConnected ////////////////////////////////////////////////////////////////////////////////////////
        Values_FullyConnected = new double * [ FullyConnectedLayers ];

        for ( int LayerIndex = 0 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            // ( + 1 ) --> Bias

            Values_FullyConnected [ LayerIndex ] = new double [ Neurons [ LayerIndex ] + 1 ];
        }

        for ( int LayerIndex = 0 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for( int NeuronIndex = 0 ; NeuronIndex < Neurons [ LayerIndex ] ; ++NeuronIndex )
            {
                Values_FullyConnected [ LayerIndex ] [ NeuronIndex ] = 0;
            }



            // Bias Value

            Values_FullyConnected [ LayerIndex ] [ Neurons [ LayerIndex ] ] = 1;
        }





        // Errors_FullyConnected ////////////////////////////////////////////////////////////////////////////////////////
        Errors_FullyConnected = new double * [ FullyConnectedLayers ];

        for ( int LayerIndex = 0 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            Errors_FullyConnected [ LayerIndex ] = new double [ Neurons [ LayerIndex ] ];
        }

        for ( int LayerIndex = 0; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            for ( int NeuronIndex = 0 ; NeuronIndex < Neurons [ LayerIndex ] ; ++NeuronIndex )
            {
                Errors_FullyConnected [ LayerIndex ] [ NeuronIndex ] = 0;
            }
        }
    }

    void PrepareFunction_07_AdjustmentVariables()
    {
        // Normalization Variables //////////////////////////////////////////////////////////////////////////////////////

        // Encoder Normalization Variables

        Exp = 0;

        Std = 0;

        for(int Word = 0; Word < FeaturesOut_Embedding_Document; ++Word) Weights_Gamma [ Word ] = Weights_Beta [ Word ] = 1;

        for(int Word = 0; Word < FeaturesOut_Embedding_Document; ++Word) Weights_Gamma_Delta [ Word ] = Weights_Beta_Delta [ Word ] = 0;

        Weights_EpsScalar = 1e-5;

        Weights_EpsScalar_Delta = 0;





        // FullyConnected Normalization Variables

        Weights_Gamma_FC = 1;

        Weights_Gamma_FC_Delta = 0;

        Weights_Beta_FC = 1;

        Weights_Beta_FC_Delta = 0;

        Weights_EpsScalar_FC = 1e-5;

        Weights_EpsScalar_FC_Delta = 0;
    }

    void PrepareFunction_08_MultiThreadingVariables()
    {
        // Multi Threading Variables ////////////////////////////////////////////////////////////////////////////////////

        ThreadsAvailable = thread::hardware_concurrency();

        ProcessesNumber = 0;

        ThreadResponsibilities = 0;

        ThreadPointer = new thread[ThreadsAvailable];

        FirstIndexOfMyResponsibilities_Thread = new unsigned int[ ThreadsAvailable ];

        LastIndexOfMyResponsibilities_Thread     = new unsigned int[ ThreadsAvailable ];

        for(int ThreadIndex = 0; ThreadIndex < ThreadsAvailable; ++ThreadIndex)
        {
            FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] = 0;
            LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] = 0;
        }
    }

    void PrepareFunction_09_MomentumRate_LearningRate()
    {
        // Each Encoder Have Special ( MomentumRate && LearningRate )
        // Earlier Encoders That Far To ClassificationLayer Have ( MomentumRate && LearningRate )    >    ( MomentumRate && LearningRate ) Of Later Encoders That Close To ClassificationLayer
        // Because Solving Vanishing GradientDescent On Earlier Layers
        double MomentumRate = 0.70, MomentumRate_Scheduling = 1.00, LearningRate = 0.30, LearningRate_Scheduling = 0.99;





        // MomentumRate_ClassificationLayer & LearningRate_ClassificationLayer //////////////////////////////////////////

        MomentumRate_ClassificationLayer = MomentumRate;

        LearningRate_ClassificationLayer = LearningRate;

        MomentumRate_ClassificationLayer_Scheduling = MomentumRate_Scheduling;

        LearningRate_ClassificationLayer_Scheduling = LearningRate_Scheduling;





        // MomentumRate_TransformerModel & LearningRate_TransformerModel ////////////////////////////////////////////////

        MomentumRate_TransformerModel = new double * [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            MomentumRate_TransformerModel [ BlockIndex ] = new double [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = BlocksOfEncoders - 1; BlockIndex >= 0; --BlockIndex)
        {
            for(int EncoderIndex = EncodersNumber_Block [ BlockIndex ] - 1; EncoderIndex >= 0 ; --EncoderIndex)
            {
                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ] = MomentumRate;

                MomentumRate = MomentumRate * 1.005;
            }
        }



        LearningRate_TransformerModel = new double * [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            LearningRate_TransformerModel [ BlockIndex ] = new double [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = BlocksOfEncoders - 1; BlockIndex >= 0; --BlockIndex)
        {
            for(int EncoderIndex = EncodersNumber_Block [ BlockIndex ] - 1; EncoderIndex >= 0 ; --EncoderIndex)
            {
                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ] = LearningRate;

                LearningRate = LearningRate * 1.005;
            }
        }



        MomentumRate_TransformerModel_Scheduling = new double * [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            MomentumRate_TransformerModel_Scheduling [ BlockIndex ] = new double [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = BlocksOfEncoders - 1; BlockIndex >= 0; --BlockIndex)
        {
            for(int EncoderIndex = EncodersNumber_Block [ BlockIndex ] - 1; EncoderIndex >= 0 ; --EncoderIndex)
            {
                MomentumRate_TransformerModel_Scheduling [ BlockIndex ] [ EncoderIndex ] = MomentumRate_Scheduling;
                // MomentumRate_TransformerModel_Scheduling ( Not Changed )    ,    We Can Use One Variable Instead of ( 2D )
            }
        }



        LearningRate_TransformerModel_Scheduling = new double * [ BlocksOfEncoders ];

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            LearningRate_TransformerModel_Scheduling [ BlockIndex ] = new double [ EncodersNumber_Block [ BlockIndex ] ];
        }

        for(int BlockIndex = BlocksOfEncoders - 1; BlockIndex >= 0; --BlockIndex)
        {
            for(int EncoderIndex = EncodersNumber_Block [ BlockIndex ] - 1; EncoderIndex >= 0 ; --EncoderIndex)
            {
                LearningRate_TransformerModel_Scheduling [ BlockIndex ] [ EncoderIndex ] = LearningRate_Scheduling;
                // LearningRate_TransformerModel_Scheduling ( Not Changed )    ,    We Can Use One Variable Instead of ( 2D )
            }
        }





        // MomentumRate_EmbeddingLayer & LearningRate_EmbeddingLayer ////////////////////////////////////////////////////

        MomentumRate_EmbeddingLayer = MomentumRate;

        LearningRate_EmbeddingLayer = LearningRate;

        MomentumRate_EmbeddingLayer_Scheduling = MomentumRate_Scheduling;

        LearningRate_EmbeddingLayer_Scheduling = LearningRate_Scheduling;
    }

    void PrepareFunction_10_SpecialHyperParameters()
    {
        AllowOfFlowingFeaturesOutBetweenBlocksDirectly = 1;
    }

    void PrepareFunction_11_TemporaryVariables()
    {
        // Temporary Variables //////////////////////////////////////////////////////////////////////////////////////////

        for(int Word = 0; Word < DocumentLengthByWord; ++Word)
        {
            Softmax_Values_Edges [ Word ] = 0;
        }

        Softmax_Values_FullyConnected=0;

        Rand0 = 0;

        Rand1 = 0;

        Tmp0 = 0;

        Tmp1 = 0;
    }





    void Training(const int &MaskingPercentage, const int &NumberOfMaskingForSameDocument)
    {
        for(int SentenceIndexAtFirst = 0 ; SentenceIndexAtFirst < (Data.SentencesByIndices.size() - DocumentLengthBySentence + 1 ) ; ++SentenceIndexAtFirst )
        {
            FirstSentenceIndexInDocument = SentenceIndexAtFirst;

            LastSentenceIndexInDocument = SentenceIndexAtFirst + DocumentLengthBySentence - 1;

            for(int SentenceIndex = FirstSentenceIndexInDocument ; SentenceIndex <= LastSentenceIndexInDocument ; ++SentenceIndex )
            {
                MaskingNumberInSentence [SentenceIndex - FirstSentenceIndexInDocument ] = double(MaskingPercentage ) / 100.0 * double (Data.SentencesByIndices [ SentenceIndex ].size() );
            }



            ForwardPropagation_00_EmbeddingLayer_00_DocumentAssigning();



            // Masking
            // Repeat The Choosing Of Random Words From Same Document == ( NumberOfMaskingForSameDocument ) Times
            for( int MaskNumber = 0 ; MaskNumber < NumberOfMaskingForSameDocument ; ++MaskNumber)
            {
                ForwardPropagation_00_EmbeddingLayer_01_Masking_00_ChooseMasks();
                ForwardPropagation_00_EmbeddingLayer_01_Masking_01_ApplyMasks();

                ForwardPropagation_01_TransformerModel_00();
                ForwardPropagation_02_ClassificationLayer_00();

                BackPropagation_00_ClassificationLayer_00();
                BackPropagation_01_TransformerModel_00();
                BackPropagation_02_EmbeddingLayer_00();

                ForwardPropagation_00_EmbeddingLayer_01_Masking_02_DeleteMasks();
                ForwardPropagation_00_EmbeddingLayer_02_ContributedValues_ContributedErrors_ZeroInitialization();
            }



            // Masking
            // Repeat The Choosing Of Random Words From Same Document == ( NumberOfMaskingForSameDocument * Number Of Words In The Document ) Times
            // Every MaskNumber Must Choose All Words In Order , It Can Leave Of Dropout()
            for( int MaskNumber = 0 ; MaskNumber < NumberOfMaskingForSameDocument ; ++MaskNumber)
            {
                // Try Replace It --> for ( WordIndex ) for ( MaskNumber )
                for( int WordIndex = 0 ; WordIndex < NumberOfWordsInCurrentDocument ; ++WordIndex)
                {
                    if(IndicesWordsInDictionary_Document [ WordIndex ] == CLS || IndicesWordsInDictionary_Document [ WordIndex ] == SEP || IndicesWordsInDictionary_Document [ WordIndex ] == PAD ) continue;
                    else Masks_Document[ WordIndex ] = 1;

                    ForwardPropagation_00_EmbeddingLayer_01_Masking_00_ChooseMasks();
                    ForwardPropagation_00_EmbeddingLayer_01_Masking_01_ApplyMasks();

                    ForwardPropagation_01_TransformerModel_00();
                    ForwardPropagation_02_ClassificationLayer_00();

                    BackPropagation_00_ClassificationLayer_00();
                    BackPropagation_01_TransformerModel_00();
                    BackPropagation_02_EmbeddingLayer_00();

                    ForwardPropagation_00_EmbeddingLayer_01_Masking_02_DeleteMasks();
                    ForwardPropagation_00_EmbeddingLayer_02_ContributedValues_ContributedErrors_ZeroInitialization();
                }
            }
        }



        Adjustment_03_SchedulingRates();
    }





    void ForwardPropagation_00_EmbeddingLayer_00_DocumentAssigning()
    {
        // Assigning Dictionary Index for All Words in Current Document --> ( IndicesWordsInDictionary_Document )
        // Assigning ( Embedding_Word [----]    Concatenate    Embedding_PositionOfWordInSentence [----] ) for All Words in Current Document --> ( Values_Embedding_Document )





        // Store Words In Document By ( Index Of Word In Dictionary )      -->    IndicesWordsInDictionary_Document [----]

        // Store Words In Document By ( Index Of Word In Sentence )        -->    IndicesWordsInSentence_Document [----]

        // Store Words In Document By ( Index Of Sentence In Document )    -->    IndicesSentenceInDocument_Document [----]

        NumberOfWordsInCurrentDocument = 0;

        LengthOfLargestSentenceInCurrentDocument = 0;

        for( int SentenceIndexInDatabase = FirstSentenceIndexInDocument ; SentenceIndexInDatabase <= LastSentenceIndexInDocument ; ++SentenceIndexInDatabase )
        {
            if ( SentenceIndexInDatabase > FirstSentenceIndexInDocument ) IndicesWordsInDictionary_Document [ NumberOfWordsInCurrentDocument ] = SEP;
            else IndicesWordsInDictionary_Document [ NumberOfWordsInCurrentDocument ] = CLS;

            IndicesWordsInSentence_Document [ NumberOfWordsInCurrentDocument ] = 0;

            IndicesSentenceInDocument_Document [ NumberOfWordsInCurrentDocument ] = SentenceIndexInDatabase - FirstSentenceIndexInDocument;

            ++NumberOfWordsInCurrentDocument;



            for ( int WordIndexInSentence = 0 ; WordIndexInSentence < Data.SentencesByIndices [ SentenceIndexInDatabase ].size() ; ++WordIndexInSentence )
            {
                IndicesWordsInDictionary_Document [ NumberOfWordsInCurrentDocument ] = Data.SentencesByIndices [ SentenceIndexInDatabase ] [ WordIndexInSentence ];

                IndicesWordsInSentence_Document [ NumberOfWordsInCurrentDocument ] = 1 + WordIndexInSentence;

                IndicesSentenceInDocument_Document [ NumberOfWordsInCurrentDocument ] = SentenceIndexInDatabase - FirstSentenceIndexInDocument;

                ++NumberOfWordsInCurrentDocument;



                if ( Data.SentencesByIndices [ SentenceIndexInDatabase ].size() > LengthOfLargestSentenceInCurrentDocument )
                {
                    LengthOfLargestSentenceInCurrentDocument = Data.SentencesByIndices [ SentenceIndexInDatabase ].size();
                }
            }
        }





        // Values_Embedding_Document [----] == Concatenate [ Embedding_Word [----]    ,    Embedding_PositionOfWordInSentence [----]    ,    Embedding_PositionOfSentenceInDocument [----]    ,    Embedding_Bias [----] ]

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
        {
            // FirstIndexIn_Embedding_Document = 0;

            // LastIndexIn_Embedding_Document = FeaturesOut_Embedding_Word;

            // Values_Embedding_Document [    FirstIndexIn_Embedding_Document    :    LastIndexIn_Embedding_Document    ]    ==    Embedding_Word [----]

            for ( int FeaturesOut_Word = 0 ; FeaturesOut_Word < FeaturesOut_Embedding_Word ; ++FeaturesOut_Word )
            {
                Values_Embedding_Document [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 0 ] + FeaturesOut_Word ] =

                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 0 ] + FeaturesOut_Word ] =

                        Embedding_Word [ IndicesWordsInDictionary_Document [ WordIndexInDocument ] ] [ FeaturesOut_Word ];
            }





            // FirstIndexIn_Embedding_Document = FeaturesOut_Embedding_Word;

            // LastIndexIn_Embedding_Document = FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence;

            // Values_Embedding_Document [    FirstIndexIn_Embedding_Document    :    LastIndexIn_Embedding_Document    ]    ==    Embedding_PositionOfWordInSentence [----]

            for ( int FeaturesOut_PositionOfWordInSentence = 0 ; FeaturesOut_PositionOfWordInSentence < FeaturesOut_Embedding_PositionOfWordInSentence ; ++FeaturesOut_PositionOfWordInSentence )
            {
                Values_Embedding_Document [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 1 ] + FeaturesOut_PositionOfWordInSentence ] =

                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 1 ] + FeaturesOut_PositionOfWordInSentence ] =

                        Embedding_PositionOfWordInSentence [ IndicesWordsInSentence_Document [ WordIndexInDocument ] ] [ FeaturesOut_PositionOfWordInSentence ];
            }





            // FirstIndexIn_Embedding_Document = FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence;

            // LastIndexIn_Embedding_Document = FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence + FeaturesOut_Embedding_PositionOfSentenceInDocument;

            // Values_Embedding_Document [    FirstIndexIn_Embedding_Document    :    LastIndexIn_Embedding_Document    ]    ==    Embedding_PositionOfSentenceInDocument [----]

            for ( int FeaturesOut_PositionOfSentenceInDocument = 0 ; FeaturesOut_PositionOfSentenceInDocument < FeaturesOut_Embedding_PositionOfSentenceInDocument ; ++FeaturesOut_PositionOfSentenceInDocument )
            {
                Values_Embedding_Document [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 2 ] + FeaturesOut_PositionOfSentenceInDocument ] =

                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 2 ] + FeaturesOut_PositionOfSentenceInDocument ] =

                        Embedding_PositionOfSentenceInDocument [ IndicesSentenceInDocument_Document [ WordIndexInDocument ] ] [ FeaturesOut_PositionOfSentenceInDocument ];
            }





            // FirstIndexIn_Embedding_Document = FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence + FeaturesOut_Embedding_PositionOfSentenceInDocument;

            // LastIndexIn_Embedding_Document = FeaturesOut_Embedding_Word + FeaturesOut_Embedding_PositionOfWordInSentence + FeaturesOut_Embedding_PositionOfSentenceInDocument + FeaturesOut_Embedding_Bias;

            // Values_Embedding_Document [    FirstIndexIn_Embedding_Document    :    LastIndexIn_Embedding_Document    ]    ==    Embedding_Bias [----]

            for (int FeaturesOut_Bias = 0 ; FeaturesOut_Bias < FeaturesOut_Embedding_Bias ; ++FeaturesOut_Bias )
            {
                Values_Embedding_Document [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 3 ] + FeaturesOut_Bias ] =

                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 3 ] + FeaturesOut_Bias ] =

                        Embedding_Bias [ FeaturesOut_Bias ];
            }
        }
    }

    void ForwardPropagation_00_EmbeddingLayer_01_Masking_00_ChooseMasks()
    {
        int NumberOfWordsInPastSentences = 0;

        for( int SentenceIndexInDatabase = FirstSentenceIndexInDocument ; SentenceIndexInDatabase <= LastSentenceIndexInDocument ; ++SentenceIndexInDatabase )
        {
            for ( int MaskIndex = 0 ; MaskIndex < MaskingNumberInSentence [ SentenceIndexInDatabase - FirstSentenceIndexInDocument ] ; ++MaskIndex )
            {
                // if ( Rand0 == 0 ) --> ( CLS || SEP )

                Rand0 = rand() % Data.SentencesByIndices [ SentenceIndexInDatabase ].size() + 1;

                Masks_Document [ NumberOfWordsInPastSentences + Rand0 ] = 1;
            }



            // ( + 1 ) For Special Token ( CLS || SEP ) Added Before Sentence

            NumberOfWordsInPastSentences += ( 1 + Data.SentencesByIndices [ SentenceIndexInDatabase ].size() );
        }
    }

    void ForwardPropagation_00_EmbeddingLayer_01_Masking_01_ApplyMasks()
    {
        for (int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
        {
            if ( Masks_Document [ WordIndexInDocument ] == 1 )
            {
                // ( 80 % ) Replace Correct Word By    -->    [ MASK ]
                // ( 10 % ) Replace Correct Word By    -->    Random Word
                // ( 10 % ) Not Replace Correct Word
                Rand0 = rand() % 100;



                // ( 80 % ) Replace Correct Word By    -->    [ MASK ]
                if ( Rand0 < 80 )
                {
                    for ( int FeaturesOut_Word = 0 ; FeaturesOut_Word < FeaturesOut_Embedding_Word ; ++FeaturesOut_Word )
                    {
                        Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesOut_Word ] = Embedding_Word [ MASK ] [ FeaturesOut_Word ];
                    }
                }



                // ( 10 % ) Replace Correct Word By    -->    Random Word
                else if ( Rand0 < 90 )
                {
                    // Choose Random Word For Option
                    Rand1 = rand() % Data.NumberOfWords;



                    for ( int FeaturesOut_Word = 0 ; FeaturesOut_Word < FeaturesOut_Embedding_Word ; ++FeaturesOut_Word )
                    {
                        Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesOut_Word ] = Embedding_Word [ Rand1 ] [ FeaturesOut_Word ];
                    }
                }



                // ( 10 % ) Not Replace Correct Word
                // Not Change Anything
            }
        }
    }

    void ForwardPropagation_00_EmbeddingLayer_01_Masking_02_DeleteMasks()
    {
        for (int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
        {
            if ( Masks_Document [ WordIndexInDocument ] == 1 )
            {
                for( int FeaturesOt_Word = 0 ; FeaturesOt_Word < FeaturesOut_Embedding_Word ; ++FeaturesOt_Word )
                {
                    Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesOt_Word ] = Values_Embedding_Document [ WordIndexInDocument ] [ FeaturesOt_Word ];
                }

                Masks_Document [ WordIndexInDocument ] = 0;
            }
        }
    }

    void ForwardPropagation_00_EmbeddingLayer_02_ContributedValues_ContributedErrors_ZeroInitialization()
    {
        // EmbeddingLayer Contributed Variables
        for (int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Embedding_Document ; ++FeaturesOut )
            {
                Errors_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut ] = 0;
            }
        }



        // InternalAttention Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                        Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // Quires Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                        Errors_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // Keys Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                        Errors_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // Edges Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int ActorWordIndexInDocument = 0; ActorWordIndexInDocument < NumberOfWordsInCurrentDocument; ++ActorWordIndexInDocument)
                {
                    for(int ReactedWordIndexInDocument = 0; ReactedWordIndexInDocument < NumberOfWordsInCurrentDocument ; ++ReactedWordIndexInDocument)
                    {
                        Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] = 0;

                        Errors_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] = 0;
                    }
                }
            }
        }



        // ExternalAttention0 Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                        Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // ExternalAttention1 Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                        Errors_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // SumDifferentVisions Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                        Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // Encoder Contributed Variables
        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ]; ++FeaturesOut)
                    {
                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] = 0;
                    }
                }
            }
        }



        // FullyConnected Contributed Variables
        for (int LayerIndex = 0; LayerIndex < FullyConnectedLayers; ++LayerIndex)
        {
            for(int NeuronIndex = 0; NeuronIndex < Neurons [ LayerIndex ]; ++NeuronIndex)
            {
                Values_FullyConnected [ LayerIndex ] [ NeuronIndex ] = 0;

                Errors_FullyConnected [ LayerIndex ] [ NeuronIndex ] = 0;
            }
        }
    }





    void ForwardPropagation_01_TransformerModel_00()
    {
        // Transformer Layers Consisted From ( Blocks ) Each Block Consisted From ( Encoders ) Each Encoder Consisted From ( Weights && Variables )

        ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities(NumberOfWordsInCurrentDocument );

        for(int BlockIndex = 0; BlockIndex < BlocksOfEncoders; ++BlockIndex)
        {
            for (int EncoderIndex = 0 ; EncoderIndex < EncodersNumber_Block [ BlockIndex ] ; ++EncoderIndex)
            {
                ForwardPropagation_01_TransformerModel_01_ValuesCollection_00_MultiThreading ( BlockIndex , EncoderIndex );



                // We Use Inner Bias Instead Of ( NormalizationLayer || SpreadingLayer )

                // Adjustment_00_NormalizationLayer_00_Matrix_Words_and_Features ( BlockIndex , EncoderIndex , NumberOfWordsInCurrentDocument , FeaturesOut_Block [ BlockIndex ] );

                // Adjustment_01_SpreadingLayer_00_Matrix_Words_and_Features ( BlockIndex , EncoderIndex , NumberOfWordsInCurrentDocument , FeaturesOut_Block [ BlockIndex ] );



                Adjustment_02_ActivationLayer_00_Activation_00_Matrix_Words_and_Features(BlockIndex, EncoderIndex,
                                                                                         NumberOfWordsInCurrentDocument,
                                                                                         FeaturesOut_Block[BlockIndex],
                                                                                         LeakyReLU);
            }
        }
    }

    void ForwardPropagation_01_TransformerModel_01_ValuesCollection_00_MultiThreading(const int &BlockIndex, const int &EncoderIndex)
    {
        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step

        // ( ValuesInternalAttention    ,    ValuesQuires    ,    ValuesKeys    ,    ValuesExternalAttention0 )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::ForwardPropagation_01_TransformerModel_01_ValuesCollection_01_ValuesInternalAttention_ValuesQuires_ValuesKeys_ValuesExternalAttention0, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();





        // ( ValuesEdges )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::ForwardPropagation_01_TransformerModel_01_ValuesCollection_02_ValuesEdges, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();





        // ( ValuesExternalAttention1 )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::ForwardPropagation_01_TransformerModel_01_ValuesCollection_03_ValuesExternalAttention1, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();





        // ( ValuesSumDifferentVisions )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::ForwardPropagation_01_TransformerModel_01_ValuesCollection_04_ValuesSumDifferentVisions, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();





        // ( ValuesEncoder )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::ForwardPropagation_01_TransformerModel_01_ValuesCollection_05_ValuesEncoder, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();
    }

    void ForwardPropagation_01_TransformerModel_01_ValuesCollection_01_ValuesInternalAttention_ValuesQuires_ValuesKeys_ValuesExternalAttention0(const int &BlockIndex, const int &EncoderIndex , const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        if ( EncoderIndex > 0 )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    for ( int FeaturesIn = 0 ; FeaturesIn < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesIn )
                    {
                        Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer[ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ];
                    }
                }
            }
        }

        else if ( EncoderIndex == 0 && BlockIndex > 0 )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    for ( int FeaturesIn = 0 ; FeaturesIn < FeaturesOut_Block [ BlockIndex - 1 ] ; ++FeaturesIn )
                    {
                        Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ];
                    }
                }
            }
        }

        else if ( EncoderIndex == 0 && BlockIndex == 0 )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    for ( int FeaturesIn = 0 ; FeaturesIn < FeaturesOut_Embedding_Word ; ++FeaturesIn )
                    {
                        Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ];



                        Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ]
                            *
                                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ];
                    }
                }
            }
        }
    }

    void ForwardPropagation_01_TransformerModel_01_ValuesCollection_02_ValuesEdges(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        for ( int ActorWordIndexInDocument = FirstIndexOfMyResponsibilities ; ActorWordIndexInDocument <= LastIndexOfMyResponsibilities ; ++ActorWordIndexInDocument )
        {
            Softmax_Values_Edges [ ActorWordIndexInDocument ] = 0;



            for ( int ReactedWordIndexInDocument = 0 ; ReactedWordIndexInDocument < NumberOfWordsInCurrentDocument ; ++ReactedWordIndexInDocument )
            {
                Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] = 0;

                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] +=

                        Values_Quires [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ FeaturesOut ]
                    *
                        Values_Keys [ BlockIndex ] [ EncoderIndex ] [ ReactedWordIndexInDocument ] [ FeaturesOut ];
                }

                Softmax_Values_Edges [ ActorWordIndexInDocument ] += Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ];
            }



            // Way1 For Apply Softmax For Edges ( Sum Of All Relations For Any Word == 1 )
            for ( int ReactedWordIndexInDocument = 0 ; ReactedWordIndexInDocument < NumberOfWordsInCurrentDocument ; ++ReactedWordIndexInDocument )
            {
                Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] /= Softmax_Values_Edges [ ActorWordIndexInDocument ];
            }


            /*
            // Way1 For Apply Softmax For Edges ( Decrease All Relations For Any Word --> Division Each Relation By FeaturesOut^0.5 )
            for( int ReactedWordIndexInDocument = 0 ; ReactedWordIndexInDocument < NumberOfWordsInCurrentDocument ; ++ReactedWordIndexInDocument )
            {
                Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] /= pow(FeaturesOut_Block [ BlockIndex ] , 0.5);
            }
             */
        }
    }

    void ForwardPropagation_01_TransformerModel_01_ValuesCollection_03_ValuesExternalAttention1(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        for ( int ActorWordIndexInDocument = FirstIndexOfMyResponsibilities ; ActorWordIndexInDocument <= LastIndexOfMyResponsibilities ; ++ActorWordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
            {
                Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ FeaturesOut ] = 0;

                for ( int ReactedWordIndexInDocument = 0 ; ReactedWordIndexInDocument < NumberOfWordsInCurrentDocument; ++ReactedWordIndexInDocument )
                {
                    Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ FeaturesOut ] +=

                            Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ]
                        *
                            Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ ReactedWordIndexInDocument ] [ FeaturesOut ];
                }
            }
        }
    }

    void ForwardPropagation_01_TransformerModel_01_ValuesCollection_04_ValuesSumDifferentVisions(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        if ( EncoderIndex > 0 )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =
                            (
                                    Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                *
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                            )
                        +
                            (
                                    Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                *
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                            )
                        +
                            (
                                    Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                *
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                            );
                }
            }
        }

        else if ( EncoderIndex == 0 && BlockIndex > 0 && FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Block [ BlockIndex - 1 ] && AllowOfFlowingFeaturesOutBetweenBlocksDirectly )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =
                        (
                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        )
                    +
                        (
                                Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        )
                    +
                        (
                                Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        );
                }
            }
        }

        else if ( EncoderIndex == 0 && BlockIndex > 0 && FeaturesOut_Block [ BlockIndex ] != FeaturesOut_Block [ BlockIndex - 1 ] )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Values_SumDifferentVisions[BlockIndex][EncoderIndex][WordIndexInDocument][FeaturesOut] =

                        // Dimension of Values_Encoder [ BlockIndex - 1 ] [ Last Encoder ]    !=    Dimension of Values_SumDifferent [ BlockIndex ] [ Encoder == 0 ]
                        // So We Not Allow Values_Encoder [ BlockIndex - 1 ] [ Last Encoder ] To Contribute in Values_SumDifferent [ BlockIndex ] [ Encoder == 0 ] in ForwardPropagation
                        // So We Not Allow Errors_Encoder [ BlockIndex - 1 ] [ Last Encoder ] To Collect Errors From Errors_SumDifferent [ BlockIndex ] [ Encoder == 0 ] in BackPropagationPropagation

                        /*

                        (
                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        )
                    +
                        */

                        (
                                Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        )
                    +
                        (
                                Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        );
                }
            }
        }

        else if ( EncoderIndex == 0 && BlockIndex == 0 && FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Embedding_Document  && AllowOfFlowingFeaturesOutBetweenBlocksDirectly )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                    for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =
                        (
                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        )
                    +
                        (
                                Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        )
                    +
                        (
                                Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                        );
                }
            }
        }

        else if ( EncoderIndex == 0 && BlockIndex == 0 && FeaturesOut_Block [ BlockIndex ] != FeaturesOut_Embedding_Document )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =

                    // Dimension of FeaturesOut_Embedding_Document    !=    Dimension of Values_SumDifferent [ BlockIndex == 0 ] [ Encoder == 0 ]
                    // So We Not Allow Values_Embedding_Document To Contribute in Values_SumDifferent [ BlockIndex == 0 ] [ Encoder == 0 ] in ForwardPropagation
                    // So We Not Allow Errors_Embedding_Document To Collect Errors From Errors_SumDifferent [ BlockIndex == 0 ] [ Encoder == 0 ] in BackPropagationPropagation

                    /*

                    (
                            Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                    )
                +
                     */

                    (
                            Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                    )
                +
                    (
                            Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                    );
                }
            }
        }
    }

    void ForwardPropagation_01_TransformerModel_01_ValuesCollection_05_ValuesEncoder(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
            {
                for ( int FeaturesIn = 0; FeaturesIn < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesIn )
                {
                    Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                            Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesIn ]
                        *
                            Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesIn ] [ FeaturesOut ];
                }
            }
        }
    }





    void ForwardPropagation_02_ClassificationLayer_00()
    {
        // Classification Layers consisted from ( Fully Connected Layers ) each layer consisted from ( Weights && Variables )

        for (int LayerIndex = 0 ; LayerIndex < FullyConnectedLayers ; ++LayerIndex )
        {
            ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities (Neurons [ LayerIndex ] );

            ForwardPropagation_02_ClassificationLayer_01_ValuesCollection_00_MultiThreading (LayerIndex );


            // We Use Inner Bias Instead Of ( NormalizationLayer || SpreadingLayer )

            // Adjustment_00_NormalizationLayer_01_Vector_Neurons (LayerIndex );

            // Adjustment_01_SpreadingLayer_01_Vector_Neurons ( LayerIndex );


            if (LayerIndex < FullyConnectedLayers - 1 )
            {
                Adjustment_02_ActivationLayer_00_Activation_01_Vector_Neurons(LayerIndex, LeakyReLU);

            }

            else // if (LayerIndex == FullyConnectedLayers - 1 )
            {
                Adjustment_02_ActivationLayer_00_Activation_01_Vector_Neurons(LayerIndex, Softmax);
            }
        }
    }

    void ForwardPropagation_02_ClassificationLayer_01_ValuesCollection_00_MultiThreading(const int &LayerIndex)
    {
        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::ForwardPropagation_02_ClassificationLayer_01_ValuesCollection_01_ValuesFullyConnected, *this,
                                                   LayerIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();
    }

    void ForwardPropagation_02_ClassificationLayer_01_ValuesCollection_01_ValuesFullyConnected(const int &LayerIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        if ( LayerIndex > 0 )
        {
            for ( int NeuronIndexOfCurrentLayer = FirstIndexOfMyResponsibilities ; NeuronIndexOfCurrentLayer < LastIndexOfMyResponsibilities ; ++NeuronIndexOfCurrentLayer )
            {
                for ( int NeuronIndexOfPreviousLayer = 0 ; NeuronIndexOfPreviousLayer < Neurons [ LayerIndex - 1 ] + 1 ; ++NeuronIndexOfPreviousLayer )
                {
                    // ( + 1 ) --> Bias

                    Values_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] +=

                            Values_FullyConnected [ LayerIndex - 1 ] [ NeuronIndexOfPreviousLayer ]
                        *
                            Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ];
                }
            }
        }

        else if ( LayerIndex == 0 )
        {
            for ( int NeuronIndexOfCurrentLayer = FirstIndexOfMyResponsibilities ; NeuronIndexOfCurrentLayer < LastIndexOfMyResponsibilities ; ++NeuronIndexOfCurrentLayer )
            {
                for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
                {
                    for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlocksOfEncoders - 1 ] ; ++FeaturesOut )
                    {
                        Values_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] +=

                                Values_Encoder [ BlocksOfEncoders - 1 ] [ EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ WordIndexInDocument * FeaturesOut_Block [ BlocksOfEncoders - 1 ] + FeaturesOut ];
                    }
                }
            }
        }
    }





    void BackPropagation_00_ClassificationLayer_00()
    {
        BackPropagation_00_ClassificationLayer_01_ErrorsCollection_00();

        BackPropagation_00_ClassificationLayer_02_WeightsModification_00();
    }

    void BackPropagation_00_ClassificationLayer_01_ErrorsCollection_00()
    {
        // Classification Layers Consisted From ( Fully Connected Layers ) Each Layer Consisted From ( Weights && Variables )



        // Errors of Neurons in LastSentenceIndexInDocument Layer ( Neurons That Carry The Result )
        for ( int NeuronIndex = 0 ; NeuronIndex < Neurons [ FullyConnectedLayers - 1 ] ; ++NeuronIndex )
        {
            Errors_FullyConnected [ FullyConnectedLayers - 1 ] [ NeuronIndex ] = Values_FullyConnected [ FullyConnectedLayers - 1 ] [ NeuronIndex ] - Correct;
        }

        // Errors Calculation for Classification Layer
        for ( int LayerIndex = FullyConnectedLayers - 2 ; LayerIndex >= 0 ; --LayerIndex )
        {
            ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities(Neurons [ LayerIndex ] );

            BackPropagation_00_ClassificationLayer_01_ErrorsCollection_01_MultiThreading ( LayerIndex );
        }
    }

    void BackPropagation_00_ClassificationLayer_01_ErrorsCollection_01_MultiThreading(const int &LayerIndex)
    {
        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step

        for ( int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_00_ClassificationLayer_01_ErrorsCollection_02_ErrorsFullyConnected, *this,
                                                   LayerIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for ( int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();
    }

    void BackPropagation_00_ClassificationLayer_01_ErrorsCollection_02_ErrorsFullyConnected(const int &LayerIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        for ( int NeuronIndexOfCurrentLayer = FirstIndexOfMyResponsibilities ; NeuronIndexOfCurrentLayer <= LastIndexOfMyResponsibilities ; ++NeuronIndexOfCurrentLayer )
        {
            for ( int NeuronIndexOfNextLayer = 0 ; NeuronIndexOfNextLayer < Neurons [ LayerIndex + 1 ] ; ++NeuronIndexOfNextLayer )
            {
                Errors_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] +=

                        Errors_FullyConnected [ LayerIndex + 1 ] [ NeuronIndexOfNextLayer ]
                    *
                        (
                                Weights_FullyConnected [ LayerIndex + 1 ] [ NeuronIndexOfNextLayer ] [ NeuronIndexOfCurrentLayer ]
                            /
                                Weights_FullyConnected_Sum [ LayerIndex + 1 ] [ NeuronIndexOfNextLayer ]
                        );
            }
        }
    }

    void BackPropagation_00_ClassificationLayer_02_WeightsModification_00()
    {
        // Classification Layers Consisted From ( Fully Connected Layers ) Each Layer Consisted From ( Weights && Variables )



        // Weights Modification for Classification Layer
        for ( int LayerIndex = FullyConnectedLayers - 1 ; LayerIndex >= 0 ; --LayerIndex )
        {
            ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities (Neurons [ LayerIndex ] );

            BackPropagation_00_ClassificationLayer_02_WeightsModification_01_MultiThreading ( LayerIndex );
        }
    }

    void BackPropagation_00_ClassificationLayer_02_WeightsModification_01_MultiThreading(const int &LayerIndex)
    {
        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step

        for(int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_00_ClassificationLayer_02_WeightsModification_02_WeightsFullyConnected, *this,
                                                   LayerIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }
    }

    void BackPropagation_00_ClassificationLayer_02_WeightsModification_02_WeightsFullyConnected(const int &LayerIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        double NewDelta = 0;



            if ( LayerIndex > 0 )
            {
                for ( int NeuronIndexOfCurrentLayer = FirstIndexOfMyResponsibilities ; NeuronIndexOfCurrentLayer <= LastIndexOfMyResponsibilities ; ++NeuronIndexOfCurrentLayer )
                {
                    // ( + 1 ) --> Bias
                    for ( int NeuronIndexOfPreviousLayer = 0 ; NeuronIndexOfPreviousLayer < Neurons [ LayerIndex - 1 ] + 1 ; ++NeuronIndexOfPreviousLayer )
                    {

                        if ( LayerIndex < FullyConnectedLayers - 1 )
                        {
                            NewDelta =
                                    ( 2 * Errors_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] )
                                *
                                    Adjustment_02_ActivationLayer_01_Derivation_00 (Values_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] ,LeakyReLU )
                                *
                                    Values_FullyConnected [ LayerIndex - 1 ] [ NeuronIndexOfPreviousLayer ];
                        }

                        else // if ( LayerIndex == FullyConnectedLayers - 1 )
                        {
                            NewDelta =
                                    ( 2 * Errors_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] )
                                *
                                    Adjustment_02_ActivationLayer_01_Derivation_00 (Values_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] , Softmax )
                                *
                                    Values_FullyConnected [ LayerIndex - 1 ] [ NeuronIndexOfPreviousLayer ];
                        }





                        Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] =
                                (
                                        Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ]
                                    *
                                        MomentumRate_ClassificationLayer
                                )
                            +
                                (
                                        NewDelta
                                    *
                                        LearningRate_ClassificationLayer
                                );





                        Weights_FullyConnected_Sum [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] -=

                                abs(Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] );



                        Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ]  -=

                                Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ];



                        Weights_FullyConnected_Sum [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] +=

                                abs(Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] );
                    }
                }
            }



            else // if ( LayerIndex == 0 )
            {
                int NeuronIndexOfPreviousLayer = 0;

                for ( int NeuronIndexOfCurrentLayer = FirstIndexOfMyResponsibilities ; NeuronIndexOfCurrentLayer <= LastIndexOfMyResponsibilities ; ++NeuronIndexOfCurrentLayer )
                {
                    for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
                    {
                        for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlocksOfEncoders - 1 ] ; ++FeaturesOut )
                        {
                            NeuronIndexOfPreviousLayer = ( WordIndexInDocument * FeaturesOut_Block [ BlocksOfEncoders - 1 ] ) + FeaturesOut;


                            NewDelta =
                                    ( 2 * Errors_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] )
                                *
                                    Adjustment_02_ActivationLayer_01_Derivation_00 (Values_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] ,LeakyReLU )
                                *
                                    Values_Encoder [ BlocksOfEncoders - 1 ] [ EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesOut ];





                            Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] =
                                    (
                                            Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ]
                                        *
                                            MomentumRate_ClassificationLayer
                                    )
                                +
                                    (
                                            NewDelta
                                        *
                                            LearningRate_ClassificationLayer
                                    );





                            Weights_FullyConnected_Sum [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] -=

                                    abs(Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] );



                            Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ]  -=

                                    Weights_FullyConnected_Delta [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ];



                            Weights_FullyConnected_Sum [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] +=

                                    abs(Weights_FullyConnected [ LayerIndex ] [ NeuronIndexOfCurrentLayer ] [ NeuronIndexOfPreviousLayer ] );
                        }
                    }
                }
            }
    }





    void BackPropagation_01_TransformerModel_00()
    {
        BackPropagation_01_TransformerModel_01_ErrorsCollection_00();

        BackPropagation_01_TransformerModel_02_WeightsModification_00();
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_00()
    {
        ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities (NumberOfWordsInCurrentDocument );



        // Transformer Layers Consisted From ( Blocks ) Each Block Consisted From ( Encoders ) Each Encoder Consisted From ( Weights && Variables )



        // ValuesEncoder [ Last Block ] [ Last Encoder ]    ( That Have Relation With Values_FullyConnected [ First Layer ] )
        // ErrorsEncoder [ Last Block ] [ Last Encoder ]    ( Must Collect it Using Special Way )

        // Errors_Encoder

        int NeuronIndexInLastEncoderLayer = 0;

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlocksOfEncoders - 1 ] ; ++FeaturesOut )
            {
                // Store Index of Weight From ( Neuron [ NeuronIndexInFirstFullyConnectedLayer ] ) To ( Neuron [ NeuronIndexInLastEncoderLayer ] )

                NeuronIndexInLastEncoderLayer = WordIndexInDocument * ( FeaturesOut_Block [ BlocksOfEncoders - 1 ] ) + FeaturesOut;



                for ( int NeuronIndexInFirstFullyConnectedLayer = 0 ; NeuronIndexInFirstFullyConnectedLayer < Neurons [ 0 ] ; ++NeuronIndexInFirstFullyConnectedLayer )
                {
                    Errors_Encoder [ BlocksOfEncoders - 1 ] [ EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesOut ] +=

                            Errors_FullyConnected [ 0 ] [ NeuronIndexInFirstFullyConnectedLayer ]
                        *
                            (
                                    Weights_FullyConnected [ 0 ] [ NeuronIndexInFirstFullyConnectedLayer ] [ NeuronIndexInLastEncoderLayer ]
                                /
                                    Weights_FullyConnected_Sum [ 0 ] [ NeuronIndexInFirstFullyConnectedLayer ]
                            );
                }
            }
        }



        // Errors_SumDifferentVisions
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_03_ErrorsSumDifferentVisions, *this,
                                                    BlocksOfEncoders - 1 , EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // Errors_ExternalAttention1
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_04_ErrorsExternalAttention1, *this,
                                                BlocksOfEncoders - 1 , EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // Errors_InternalAttention
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_05_ErrorsInternalAttention, *this,
                                                BlocksOfEncoders - 1 , EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();


        // Errors_EncoderOfPreviousEncoderLayer
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_06_ErrorsEncoderOfPreviousEncoderLayer, *this,
                                                BlocksOfEncoders - 1 , EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // Errors_Edges    ,    Errors_ExternalAttention0
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_07_ErrorsEdges_ErrorsExternalAttention0, *this,
                                                BlocksOfEncoders - 1 , EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // Errors_Quires    ,    Errors_Keys
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_08_ErrorsQuires_ErrorsKeys, *this,
                                                BlocksOfEncoders - 1 , EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 1 , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();




        // Errors Collection For All Encoder In Last Block    ,    Otherwise Last Encoder Because We Collected Above
        /********************************************/
            for ( int EncoderIndex = EncodersNumber_Block [ BlocksOfEncoders - 1 ] - 2 ; EncoderIndex >= 0 ; --EncoderIndex )
            {
                BackPropagation_01_TransformerModel_01_ErrorsCollection_01_MultiThreading (BlocksOfEncoders - 1 , EncoderIndex );
            }

        // Errors Collection For All Encoders In All Blocks Otherwise LastSentenceIndexInDocument Block
        for ( int BlockIndex = BlocksOfEncoders - 2 ; BlockIndex >= 0 ; --BlockIndex )
        {
            for ( int EncoderIndex = EncodersNumber_Block [ BlockIndex ] - 1; EncoderIndex >= 0 ; --EncoderIndex )
            {
                BackPropagation_01_TransformerModel_01_ErrorsCollection_01_MultiThreading ( BlockIndex , EncoderIndex );
            }
        }
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_01_MultiThreading(const int &BlockIndex, const int &EncoderIndex)
    {
        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step



        // ErrorsEncoder
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer[ThreadIndex] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_02_ErrorsEncoder, *this,
                                                BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // ErrorsSumDifferentVisions
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer[ThreadIndex] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_03_ErrorsSumDifferentVisions, *this,
                                                BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // ErrorsExternalAttention1
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer[ThreadIndex] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_04_ErrorsExternalAttention1, *this,
                                                BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // ErrorsInternalAttention
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer[ThreadIndex] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_05_ErrorsInternalAttention, *this,
                                                BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // ErrorsEncoderOfPreviousEncoderLayer
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer[ThreadIndex] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_06_ErrorsEncoderOfPreviousEncoderLayer, *this,
                                                BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // ErrorsEdges    ,    ErrorsExternalAttention0
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer[ThreadIndex] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_07_ErrorsEdges_ErrorsExternalAttention0, *this,
                                                BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();



        // ErrorsQuires    ,    ErrorsKeys
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer[ThreadIndex] = thread(&TransformerModel::BackPropagation_01_TransformerModel_01_ErrorsCollection_08_ErrorsQuires_ErrorsKeys, *this,
                                                BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_02_ErrorsEncoder(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        if ( EncoderIndex < EncodersNumber_Block [ BlockIndex ] - 1 )
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut_Errors = 0; FeaturesOut_Errors < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut_Errors )
                {
                    for ( int FeaturesOut_Weights = 0; FeaturesOut_Weights < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut_Weights )
                    {
                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_InternalAttention [ BlockIndex ] [ EncoderIndex + 1 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_InternalAttention [ BlockIndex ] [ EncoderIndex + 1 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex ] [ EncoderIndex + 1 ] [ 0 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex + 1 ] [ 0 ] [ FeaturesOut_Weights ]
                                );





                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_Quires [ BlockIndex ] [ EncoderIndex + 1 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_Quires [ BlockIndex ] [ EncoderIndex + 1 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex ] [ EncoderIndex + 1 ] [ 1 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex + 1 ] [ 1 ] [ FeaturesOut_Weights ]
                                );





                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_Keys [ BlockIndex ] [ EncoderIndex + 1 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_Keys [ BlockIndex ] [ EncoderIndex + 1 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex ] [ EncoderIndex + 1 ] [ 2 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex + 1 ] [ 2 ] [ FeaturesOut_Weights ]
                                );





                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex + 1 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex + 1 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex ] [ EncoderIndex + 1 ] [ 3 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex + 1 ] [ 3 ] [ FeaturesOut_Weights ]
                                );
                    }
                }
            }
        }



        else // if ( EncoderIndex == EncodersNumber_Block[ BlockIndex ] - 1 )    // Last Encoder in The Block
        {
            for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut_Errors = 0 ; FeaturesOut_Errors < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut_Errors )
                {
                    for ( int FeaturesOut_Weights = 0 ; FeaturesOut_Weights < FeaturesOut_Block [ BlockIndex + 1 ] ; ++FeaturesOut_Weights )
                    {
                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_InternalAttention [ BlockIndex + 1 ] [ EncoderIndex == 0 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_InternalAttention [ BlockIndex + 1 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex + 1 ] [ 0 ] [ 0 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex + 1 ] [ 0 ] [ 0 ] [ FeaturesOut_Weights ]
                                );





                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_Quires [ BlockIndex + 1 ] [ EncoderIndex == 0 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_Quires [ BlockIndex + 1 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex + 1 ] [ 0 ] [ 1 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex + 1 ] [ 0 ] [ 1 ] [ FeaturesOut_Weights ]
                                );





                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_Keys [ BlockIndex + 1] [ EncoderIndex == 0 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_Keys [ BlockIndex + 1 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex + 1 ] [ 0 ] [ 2 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex + 1 ] [ 0 ] [ 2 ] [ FeaturesOut_Weights ]
                                );





                        // Errors_Encoder [ BlockIndex ] [ EncoderIndex ] Come From Errors_ExternalAttention0 [ BlockIndex + 1 ] [ EncoderIndex == 0 ]

                        Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                                Errors_ExternalAttention0 [ BlockIndex + 1 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                            *
                                (
                                        Weights_LinearLayer [ BlockIndex + 1 ] [ 0 ] [ 3 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                    /
                                        Weights_LinearLayer_Sum [ BlockIndex + 1 ] [ 0 ] [ 3 ] [ FeaturesOut_Weights ]
                                );
                    }
                }
            }
        }

    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_03_ErrorsSumDifferentVisions(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut_Errors = 0 ; FeaturesOut_Errors < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut_Errors )
            {
                for ( int FeaturesOut_Weights = 0 ; FeaturesOut_Weights < FeaturesOut_Block[BlockIndex] ; ++FeaturesOut_Weights )
                {
                    Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                            Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                        *
                            (
                                    Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                /
                                    Weights_LinearLayer_Sum [ BlockIndex] [ EncoderIndex ] [ 4 ] [ FeaturesOut_Weights ]
                            );
                }
            }
        }
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_04_ErrorsExternalAttention1(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        if
        (
                EncoderIndex > 0
            ||
                (
                        EncoderIndex == 0
                    &&
                        BlockIndex > 0
                    &&
                        FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Block [ BlockIndex - 1 ]
                    &&
                        AllowOfFlowingFeaturesOutBetweenBlocksDirectly
                )
            ||
                (
                        EncoderIndex == 0
                    &&
                        BlockIndex == 0
                    &&
                        FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Embedding_Document
                    &&
                        AllowOfFlowingFeaturesOutBetweenBlocksDirectly
                )
        )
        {
            for (int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument < LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Errors_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =

                            Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }



        else
        {
            for (int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument < LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block[BlockIndex] ; ++FeaturesOut )
                {
                    Errors_ExternalAttention1 [ BlockIndex] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =

                            Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_05_ErrorsInternalAttention(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        if
        (
                EncoderIndex > 0
            ||
                (
                        EncoderIndex == 0
                    &&
                        BlockIndex > 0
                    &&
                        FeaturesOut_Block[BlockIndex] == FeaturesOut_Block [ BlockIndex - 1 ]
                    &&
                        AllowOfFlowingFeaturesOutBetweenBlocksDirectly
                )
            ||
                (
                        EncoderIndex == 0
                    &&
                        BlockIndex == 0
                    &&
                        FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Embedding_Document
                    &&
                        AllowOfFlowingFeaturesOutBetweenBlocksDirectly
                )
        )
        {
            for (int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument < LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =

                            Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }



        else
        {
            for (int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument < LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =

                            Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_06_ErrorsEncoderOfPreviousEncoderLayer(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        if( EncoderIndex > 0 )
        {
            for (int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument < LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Errors_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesOut ] =

                            Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }





        else if ( EncoderIndex == 0  && BlockIndex > 0 && FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Block [ BlockIndex - 1 ] && AllowOfFlowingFeaturesOutBetweenBlocksDirectly )
        {
            for (int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument < LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Errors_Encoder [ BlockIndex -1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesOut ] =

                            Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }





        // else if( EncoderIndex == 0  && BlockIndex > 0 && FeaturesOut_Block[ BlockIndex ] != FeaturesOut_Block[ BlockIndex - 1 ] )
        // Dimension of Values_Encoder [ BlockIndex - 1 ] [ Last Encoder ]    !=    Dimension of Values_SumDifferent [ BlockIndex ] [ Encoder == 0 ]
        // So We Not Allow Values_Encoder [ BlockIndex - 1 ] [ Last Encoder ] To Contribute in Values_SumDifferent [ BlockIndex ] [ Encoder == 0 ] in ForwardPropagation
        // So We Not Allow Errors_Encoder [ BlockIndex - 1 ] [ Last Encoder ] To Collect Errors From Errors_SumDifferent [ BlockIndex ] [ Encoder == 0 ] in BackPropagationPropagation





        else if ( EncoderIndex == 0  && BlockIndex == 0 && FeaturesOut_Block[ BlockIndex ] == FeaturesOut_Embedding_Document && AllowOfFlowingFeaturesOutBetweenBlocksDirectly )
        {
            for (int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument < LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    Errors_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut ] =

                            Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                        +
                                            Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }





        // else if( EncoderIndex == 0  && BlockIndex == 0 && FeaturesOut_Block[ BlockIndex ] != FeaturesOut_Embedding_Document )
        // Dimension of FeaturesOut_Embedding_Document    !=    Dimension of Values_SumDifferent [ BlockIndex == 0 ] [ Encoder == 0 ]
        // So We Not Allow Values_Embedding_Document_AfterMasking To Contribute in Values_SumDifferent [ BlockIndex == 0 ] [ Encoder == 0 ] in ForwardPropagation
        // So We Not Allow Errors_Embedding_Document To Collect Errors From Errors_SumDifferent [ BlockIndex == 0 ] [ Encoder == 0 ] in BackPropagationPropagation
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_07_ErrorsEdges_ErrorsExternalAttention0(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        // Errors of Edges [ X ] + Errors of ExternalAttention0 [ Y ]

        double Errors_ErrorsEdges_ErrorsExternalAttention0 = 0;



        // Pass on All Contributed Values_Edges [ X ]

        for ( int ActorWordIndexInDocument = 0 ; ActorWordIndexInDocument < NumberOfWordsInCurrentDocument ; ++ActorWordIndexInDocument )
        {
            for (int ReactedWordIndexInDocument = FirstIndexOfMyResponsibilities ; ReactedWordIndexInDocument <= LastIndexOfMyResponsibilities ; ++ReactedWordIndexInDocument )
            {
                // Pass on All Contributed Values_ExternalAttention0 [ Y ]

                for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
                {
                    // Errors of Edges [ X ] + Errors of ExternalAttention0 [ Y ]

                    Errors_ErrorsEdges_ErrorsExternalAttention0 =

                            Errors_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ FeaturesOut ]
                        *
                            (
                                    (
                                            Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ]
                                        *
                                            Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ ReactedWordIndexInDocument ] [ FeaturesOut ]
                                    )
                                /
                                    Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ FeaturesOut ]
                            );





                    // Errors of Edges [ X ]

                    Errors_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ] +=

                            Errors_ErrorsEdges_ErrorsExternalAttention0
                        *
                            (
                                    Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ]
                                /
                                    (
                                            Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ]
                                        +
                                            Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ ReactedWordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );





                    // Errors of ExternalAttention0 [ Y ]

                    Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ ReactedWordIndexInDocument ] [ FeaturesOut ] +=

                            Errors_ErrorsEdges_ErrorsExternalAttention0
                        *
                            (
                                    Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ ReactedWordIndexInDocument ] [ FeaturesOut ]
                                /
                                    (
                                            Values_Edges [ BlockIndex ] [ EncoderIndex ] [ ActorWordIndexInDocument ] [ ReactedWordIndexInDocument ]
                                        +
                                            Values_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ ReactedWordIndexInDocument ] [ FeaturesOut ]
                                    )
                            );
                }
            }
        }
    }

    void BackPropagation_01_TransformerModel_01_ErrorsCollection_08_ErrorsQuires_ErrorsKeys(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        // Errors of Quires [ X ] + Errors of Keys [ Y ]

        double Errors_ErrorsQuires_ErrorsKeys = 0;



        // Pass on All Contributed Values_Quires [ X ]

        for (int WordIndexInDocument_Quires = FirstIndexOfMyResponsibilities ; WordIndexInDocument_Quires <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument_Quires )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
            {
                // Pass on All Contributed Values_Keys [ Y ]

                for ( int WordIndexInDocument_Keys = 0 ; WordIndexInDocument_Keys < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument_Keys )
                    {
                        // Errors of Quires [ X ] + Errors of Keys [ Y ]

                        Errors_ErrorsQuires_ErrorsKeys =

                                Errors_Edges [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument_Quires ] [ WordIndexInDocument_Keys ]
                            *
                                (
                                        (
                                                Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument_Quires ] [ FeaturesOut ]
                                            *
                                                Values_Keys [ BlockIndex ] [ LastIndexOfMyResponsibilities ] [ WordIndexInDocument_Keys ] [ FeaturesOut ]
                                        )
                                    /
                                        Values_Edges [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument_Quires ] [ WordIndexInDocument_Keys ]
                                );





                        // Errors of Quires [ X ]

                        Errors_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument_Quires ] [ FeaturesOut ] +=

                                Errors_ErrorsQuires_ErrorsKeys
                            *
                                (
                                        Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument_Quires ] [ FeaturesOut ]
                                    /
                                        (
                                                Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument_Quires ] [ FeaturesOut ]
                                            +
                                                Values_Keys [ BlockIndex ] [ LastIndexOfMyResponsibilities ] [ WordIndexInDocument_Keys ] [ FeaturesOut ]
                                        )
                                );





                        // Errors of Keys [ Y ]

                        Errors_Keys [ BlockIndex ] [ LastIndexOfMyResponsibilities ] [ WordIndexInDocument_Keys ] [ FeaturesOut ] +=

                                Errors_ErrorsQuires_ErrorsKeys
                            *
                                (
                                        Values_Keys [ BlockIndex ] [ LastIndexOfMyResponsibilities ] [ WordIndexInDocument_Keys ] [ FeaturesOut ]
                                    /
                                        (
                                                Values_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument_Quires ] [ FeaturesOut ]
                                            +
                                                Values_Keys [ BlockIndex ] [ LastIndexOfMyResponsibilities ] [ WordIndexInDocument_Keys ] [ FeaturesOut ]
                                        )
                                );
                    }
            }
        }
    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_00()
    {
        // Transformer Layers Consisted From ( Blocks ) Each Block Consisted From ( Encoders ) Each Encoder Consisted From ( Weights && Variables )



        // Weights Modification for All Encoders in All Blocks
        for ( int BlockIndex = BlocksOfEncoders - 1 ; BlockIndex >= 0 ; --BlockIndex )
        {
            // Thread Assigning Depends On Number Of Rows ( Number Of Features_Out [ BlockIndex ] )

            // Except ( EncoderIndex == 0  &&  Weights Linear Layer [ 0 : 3 ] )    ,    Thread Assigning Depends On Number Of Rows ( Number Of Features_Out [ BlockIndex - 1 ] )

            // We Handle This Exception In Suitable Time In BackPropagation_01_TransformerModel_02_WeightsModification_01_MultiThreading()

            ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities(FeaturesOut_Block [ BlockIndex ] );



            for ( int EncoderIndex = EncodersNumber_Block [ BlockIndex ] - 1 ; EncoderIndex >= 0; --EncoderIndex )
            {
                BackPropagation_01_TransformerModel_02_WeightsModification_01_MultiThreading ( BlockIndex , EncoderIndex );
            }
        }
    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_01_MultiThreading(const int &BlockIndex, const int &EncoderIndex)
    {
        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step



        // Weights for Encoder ( Weights of Linear Layer [ 4 ] )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_02_WeightsModification_02_WeightsEncoder_WeightsLinearLayer4, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();




        // Weights for SumDifferentVisions
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_02_WeightsModification_03_WeightsSumDifferentVisions, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();




        // Thread Assigning Depends On Number Of Rows ( Number Of Features_Out [ BlockIndex ] )

        // Except ( EncoderIndex == 0  &&  Weights Linear Layer [ 0 : 3 ] )    ,    Thread Assigning Depends On Number Of Rows ( Number Of Features_Out [ BlockIndex - 1 ] )

        if( EncoderIndex == 0 && BlockIndex > 0 )       ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities (FeaturesOut_Block [ BlockIndex - 1 ] );
        else if( EncoderIndex == 0 && BlockIndex == 0 ) ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities (FeaturesOut_Embedding_Document );



        // Weights for ExternalAttention0 ( Weights of Linear Layer [ 3 ] )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_02_WeightsModification_04_WeightsExternalAttention0_WeightsLinearLayer3, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();




        // Weights for Keys ( Weights of Linear Layer [ 2 ] )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_02_WeightsModification_05_WeightsKeys_WeightsLinearLayer2, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();




        // Weights for Quires ( Weights of Linear Layer [ 1 ] )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_02_WeightsModification_06_WeightsQuires_WeightsLinearLayer1, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();




        // Weights for InternalAttention ( Weights of Linear Layer [ 0 ] )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_01_TransformerModel_02_WeightsModification_07_WeightsInternalAttention_WeightsLinearLayer0, *this,
                                                   BlockIndex , EncoderIndex , FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();

    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_02_WeightsEncoder_WeightsLinearLayer4(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        double NewDelta = 0;



        for ( int FeaturesIn = FirstIndexOfMyResponsibilities ; FeaturesIn <= LastIndexOfMyResponsibilities ; ++FeaturesIn )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
            {
                NewDelta = 0;

                for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
                {
                    NewDelta +=
                            ( 2 * Errors_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                        *
                            Adjustment_02_ActivationLayer_01_Derivation_00 (Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] ,LeakyReLU )
                        *
                            Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesIn ];



                    // else if ( EncoderIndex == 0 && BlockIndex > 0 )  // Do Same Thing Because Previous Activation ( Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] ) Same Block & Same Encoder

                    // else if ( EncoderIndex == 0 && BlockIndex == 0 ) // Do Same Thing Because Previous Activation ( Values_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] ) Same Block & Same Encoder
                }





                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesIn ] [ FeaturesOut ] =
                        (
                                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesIn ] [ FeaturesOut ]
                            *
                                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        )
                    +
                        (
                                NewDelta
                            *
                                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        );





                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesOut ] -=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesIn ] [ FeaturesOut ]);



                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesIn ] [ FeaturesOut ] -=

                        Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesIn ] [ FeaturesOut ];



                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesOut ] +=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 4 ] [ FeaturesIn ] [ FeaturesOut ]);

            }
        }
    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_03_WeightsSumDifferentVisions(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        double NewDelta = 0;



        for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
            {
                // Vision [ 2 ]

                NewDelta =
                        ( 2 * Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                    *
                        Values_ExternalAttention1 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ];



                Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ] =
                        (
                                Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        )
                    +
                        (
                                NewDelta
                            *
                                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        );



                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ] -=

                        Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ WordIndexInDocument ] [ FeaturesOut ];





                // Vision [ 1 ]

                NewDelta =
                        ( 2 * Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                    *
                        Values_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ];



                Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ] =
                        (
                                Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ]
                            *
                                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        )
                    +
                        (
                                NewDelta
                            *
                                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        );



                Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ] -=

                        Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ WordIndexInDocument ] [ FeaturesOut ];





                // Vision [ 0 ] if ( EncoderIndex > 0 )

                if( EncoderIndex > 0 )
                {
                    NewDelta =
                            ( 2 * Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                        *
                            Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesOut ];



                    Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ] =
                            (
                                    Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                *
                                    MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                            )
                        +
                            (
                                    NewDelta
                                *
                                    LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                            );



                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ] -=

                            Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ];
                }



                // Vision [ 0 ] if ( EncoderIndex == 0 && BlockIndex > 0 )

                else if( EncoderIndex == 0 && BlockIndex > 0 && FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Block [ BlockIndex - 1 ] && AllowOfFlowingFeaturesOutBetweenBlocksDirectly )
                {
                    NewDelta =
                            ( 2 * Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                        *
                            Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesOut ];



                    Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ] =
                            (
                                    Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                *
                                    MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                            )
                        +
                            (
                                    NewDelta
                                *
                                    LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                            );



                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ] -=

                            Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ];
                }



                // Vision [ 0 ] if ( EncoderIndex == 0 && BlockIndex == 0 )

                else if( EncoderIndex == 0 && BlockIndex == 0 && FeaturesOut_Block [ BlockIndex ] == FeaturesOut_Embedding_Document  && AllowOfFlowingFeaturesOutBetweenBlocksDirectly )
                {
                    NewDelta =
                            ( 2 * Errors_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                        *
                            Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesOut ];



                    Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ] =
                            (
                                    Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ]
                                *
                                    MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                            )
                        +
                            (
                                    NewDelta
                                *
                                    LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                            );



                    Weights_SumDifferentVisions [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ] -=

                            Weights_SumDifferentVisions_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut ];
                }
            }
        }
    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_04_WeightsExternalAttention0_WeightsLinearLayer3(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        double NewDelta = 0;



        for ( int FeaturesIn = FirstIndexOfMyResponsibilities ; FeaturesIn <= LastIndexOfMyResponsibilities ; ++FeaturesIn )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
            {
                NewDelta = 0;

                for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument)
                {
                    if ( EncoderIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if ( EncoderIndex == 0 && BlockIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if ( EncoderIndex == 0 && BlockIndex == 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_ExternalAttention0 [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ];
                    }
                }





                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ] =
                        (
                                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ]
                            *
                                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        )
                    +
                        (
                                NewDelta
                            *
                                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        );





                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesOut ] -=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ]);



                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ] -=

                        Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ];



                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesOut ] +=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 3 ] [ FeaturesIn ] [ FeaturesOut ]);
            }
        }
    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_05_WeightsKeys_WeightsLinearLayer2(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        double NewDelta = 0;



        for ( int FeaturesIn = FirstIndexOfMyResponsibilities ; FeaturesIn <= LastIndexOfMyResponsibilities ; ++FeaturesIn )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < FeaturesOut_Block [ BlockIndex ] ; ++FeaturesOut )
            {
                NewDelta = 0;

                for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
                {
                    if ( EncoderIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if ( EncoderIndex == 0 && BlockIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if ( EncoderIndex == 0 && BlockIndex == 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_Keys [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ];
                    }
                }





                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ] =
                        (
                                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ]
                            *
                                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        )
                    +
                        (
                                NewDelta
                            *
                                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        );





                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesOut ] -=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ]);



                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ] -=

                        Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ];



                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesOut ] +=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 2 ] [ FeaturesIn ] [ FeaturesOut ]);
            }
        }
    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_06_WeightsQuires_WeightsLinearLayer1(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        double NewDelta = 0;



        for(int FeaturesIn = FirstIndexOfMyResponsibilities; FeaturesIn <= LastIndexOfMyResponsibilities; ++FeaturesIn)
        {
            for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block[ BlockIndex ]; ++FeaturesOut)
            {
                NewDelta = 0;

                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    if ( EncoderIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if( EncoderIndex == 0 && BlockIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if( EncoderIndex == 0 && BlockIndex == 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_Quires [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ];
                    }
                }





                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ] =
                        (
                                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ]
                            *
                                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        )
                    +
                        (
                                NewDelta
                            *
                                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        );





                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesOut ] -=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ]);



                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ] -=

                        Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ];



                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesOut ] +=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 1 ] [ FeaturesIn ] [ FeaturesOut ]);
            }
        }
    }

    void BackPropagation_01_TransformerModel_02_WeightsModification_07_WeightsInternalAttention_WeightsLinearLayer0(const int &BlockIndex, const int &EncoderIndex, const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        double NewDelta = 0;



        for(int FeaturesIn = FirstIndexOfMyResponsibilities; FeaturesIn <= LastIndexOfMyResponsibilities; ++FeaturesIn)
        {
            for(int FeaturesOut = 0; FeaturesOut < FeaturesOut_Block[ BlockIndex ]; ++FeaturesOut)
            {
                NewDelta = 0;

                for(int WordIndexInDocument = 0; WordIndexInDocument < NumberOfWordsInCurrentDocument; ++WordIndexInDocument)
                {
                    if ( EncoderIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex ] [ EncoderIndex - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if( EncoderIndex == 0 && BlockIndex > 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Encoder [ BlockIndex - 1 ] [ EncodersNumber_Block [ BlockIndex - 1 ] - 1 ] [ WordIndexInDocument ] [ FeaturesIn ];
                    }



                    else if( EncoderIndex == 0 && BlockIndex == 0 )
                    {
                        NewDelta +=

                                ( 2 * Errors_InternalAttention [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] )
                            *
                                Values_Embedding_Document_AfterMasking [ WordIndexInDocument ] [ FeaturesIn ];
                    }
                }





                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ] =
                        (
                                Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ]
                            *
                                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        )
                    +
                        (
                                NewDelta
                            *
                                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ]
                        );





                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesOut ] -=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ]);



                Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ] -=

                        Weights_LinearLayer_Delta [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ];



                Weights_LinearLayer_Sum [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesOut ] +=

                        abs(Weights_LinearLayer [ BlockIndex ] [ EncoderIndex ] [ 0 ] [ FeaturesIn ] [ FeaturesOut ]);
            }
        }
    }





    void BackPropagation_02_EmbeddingLayer_00()
    {
        BackPropagation_02_EmbeddingLayer_01_ErrorsCollection_00_MultiThreading();

        BackPropagation_02_EmbeddingLayer_02_EmbeddingModification_00_MultiThreading();
    }

    void BackPropagation_02_EmbeddingLayer_01_ErrorsCollection_00_MultiThreading()
    {
        ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities (NumberOfWordsInCurrentDocument );

        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step



        // Errors_Embedding_Document
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_02_EmbeddingLayer_01_ErrorsCollection_01_ErrorsEmbeddingDocument, *this,
                                                   FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] , LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();
    }

    void BackPropagation_02_EmbeddingLayer_01_ErrorsCollection_01_ErrorsEmbeddingDocument(const int &FirstIndexOfMyResponsibilities, const int &LastIndexOfMyResponsibilities)
    {
        for ( int WordIndexInDocument = FirstIndexOfMyResponsibilities ; WordIndexInDocument <= LastIndexOfMyResponsibilities ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut_Errors = 0 ; FeaturesOut_Errors < FeaturesOut_Embedding_Document ; ++FeaturesOut_Errors )
            {
                for ( int FeaturesOut_Weights = 0 ; FeaturesOut_Weights < FeaturesOut_Block [ 0 ] ; ++FeaturesOut_Weights )
                {
                    // Errors_Embedding_Document Come from Errors_InternalAttention [ BlockIndex == 0 ] [ EncoderIndex == 0 ]

                    Errors_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                            Errors_InternalAttention [ 0 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                        *
                            (
                                    Weights_LinearLayer [ 0 ] [ 0 ] [ 0 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                /
                                    Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ 0 ] [ FeaturesOut_Weights ]
                            );





                    // Errors_Embedding_Document Come from Errors_Quires [ BlockIndex == 0 ] [ EncoderIndex == 0 ]

                    Errors_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                            Errors_Quires [ 0 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                        *
                            (
                                    Weights_LinearLayer [ 0 ] [ 0 ] [ 1 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                /
                                    Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ 1 ] [ FeaturesOut_Weights ]
                            );





                    // Errors_Embedding_Document Come from Errors_Keys [ BlockIndex == 0 ] [ EncoderIndex == 0 ]

                    Errors_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                            Errors_Keys [ 0 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                        *
                            (
                                    Weights_LinearLayer [ 0 ] [ 0 ] [ 2 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                /
                                    Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ 2 ] [ FeaturesOut_Weights ]
                            );





                    // Errors_Embedding_Document Come from ExternalAttention0 [ BlockIndex == 0 ] [ EncoderIndex == 0 ]

                    Errors_Embedding_Document [ WordIndexInDocument ] [ FeaturesOut_Errors ] +=

                            Errors_ExternalAttention0 [ 0 ] [ 0 ] [ WordIndexInDocument ] [ FeaturesOut_Weights ]
                        *
                            (
                                    Weights_LinearLayer [ 0 ] [ 0 ] [ 3 ] [ FeaturesOut_Errors ] [ FeaturesOut_Weights ]
                                /
                                    Weights_LinearLayer_Sum [ 0 ] [ 0 ] [ 3 ] [ FeaturesOut_Weights ]
                            );
                }
            }
        }

    }

    void BackPropagation_02_EmbeddingLayer_02_EmbeddingModification_00_MultiThreading()
    {
        ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities (NumberOfWordsInCurrentDocument );

        // Must Join All Threads For Compiler Waiting For Finish This Threads Before Calculate Next Step



        // Modification of ( Embedding_Word , Embedding_PositionOfWordInSentence )
        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex )
        {
            ThreadPointer [ ThreadIndex ] = thread(&TransformerModel::BackPropagation_02_EmbeddingLayer_02_EmbeddingModification_01_EmbeddingWord_EmbeddingPositionOfWordInSentence_EmbeddingPositionOfSentenceInDocument, *this );
        }

        for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable && ThreadIndex < ProcessesNumber ; ++ThreadIndex ) ThreadPointer [ ThreadIndex ].join();
    }

    void BackPropagation_02_EmbeddingLayer_02_EmbeddingModification_01_EmbeddingWord_EmbeddingPositionOfWordInSentence_EmbeddingPositionOfSentenceInDocument()
    {
        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < NumberOfWordsInCurrentDocument ; ++WordIndexInDocument )
        {
            // Modify Embedding_Word Depends On Errors_Embedding_Document

            for ( int FeaturesOut_Word = 0 ; FeaturesOut_Word < FeaturesOut_Embedding_Word ; ++FeaturesOut_Word )
            {
                Embedding_Word_Delta [ IndicesWordsInDictionary_Document [ WordIndexInDocument ] ] [ FeaturesOut_Word ] =
                        (
                                Embedding_Word_Delta [ IndicesWordsInDictionary_Document [ WordIndexInDocument ] ] [ FeaturesOut_Word ]
                            *
                                MomentumRate_EmbeddingLayer
                        )
                    +
                        (
                                Errors_Embedding_Document [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 0 ] + FeaturesOut_Word ]
                            *
                                LearningRate_EmbeddingLayer
                        );



                Embedding_Word [ IndicesWordsInDictionary_Document [ WordIndexInDocument ] ] [ FeaturesOut_Word ] -=

                        Embedding_Word_Delta [ IndicesWordsInDictionary_Document [ WordIndexInDocument ] ] [ FeaturesOut_Word ];
            }





            // Modify Embedding_PositionOfWordInSentence Depends On Errors_Embedding_Document

            for ( int FeaturesOut_PositionOfWordInSentence = 0 ; FeaturesOut_PositionOfWordInSentence < FeaturesOut_Embedding_PositionOfWordInSentence ; ++FeaturesOut_PositionOfWordInSentence )
            {
                Embedding_PositionOfWordInSentence_Delta [ IndicesWordsInSentence_Document [ WordIndexInDocument ] ] [ FeaturesOut_PositionOfWordInSentence ] =
                        (
                                Embedding_PositionOfWordInSentence_Delta [ IndicesWordsInSentence_Document [ WordIndexInDocument ] ] [ FeaturesOut_PositionOfWordInSentence ]
                            *
                                MomentumRate_EmbeddingLayer
                        )
                    +
                        (
                                Errors_Embedding_Document [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 1 ] + FeaturesOut_PositionOfWordInSentence ]
                            *
                                LearningRate_EmbeddingLayer
                        );



                // Position Of Word In Sentence Repeated Many Times So We Apply ( Delta Changing ) At The End
            }





            // Modify Embedding_PositionOfSentenceInDocument Depends On Errors_Embedding_Document

            for (int FeaturesOut_PositionOfSentenceInDocument = 0 ; FeaturesOut_PositionOfSentenceInDocument < FeaturesOut_Embedding_PositionOfSentenceInDocument ; ++FeaturesOut_PositionOfSentenceInDocument )
            {
                Embedding_PositionOfSentenceInDocument_Delta [ IndicesSentenceInDocument_Document [ WordIndexInDocument ] ] [ FeaturesOut_PositionOfSentenceInDocument ] =
                        (
                                Embedding_PositionOfSentenceInDocument_Delta [ IndicesSentenceInDocument_Document [ WordIndexInDocument ] ] [ FeaturesOut_PositionOfSentenceInDocument ]
                            *
                                MomentumRate_EmbeddingLayer
                        )
                    +
                        (
                                Errors_Embedding_Document [ WordIndexInDocument ] [ FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 2 ] + FeaturesOut_PositionOfSentenceInDocument ]
                            *
                                LearningRate_EmbeddingLayer
                        );



                // Position Of Sentence In Document Repeated Very Many Times So We Apply ( Delta Changing ) At The End
            }
        }





        // Position Of Word In Sentence Repeated Many Times So We Apply ( Delta Changing ) At The End

        for (int WordIndexInSentence = 0 ; WordIndexInSentence < LengthOfLargestSentenceInCurrentDocument ; ++WordIndexInSentence )
        {
            for ( int FeaturesOut_PositionOfWordInSentence = 0 ; FeaturesOut_PositionOfWordInSentence < FeaturesOut_Embedding_PositionOfWordInSentence ; ++FeaturesOut_PositionOfWordInSentence )
            {
                Embedding_PositionOfWordInSentence [ WordIndexInSentence ] [ FeaturesOut_PositionOfWordInSentence ] -=

                        Embedding_PositionOfWordInSentence_Delta [ WordIndexInSentence ] [ FeaturesOut_PositionOfWordInSentence ];
            }
        }





        // Position Of Sentence In Document Repeated Very Many Times So We Apply ( Delta Changing ) At The End

        for ( int SentenceIndexInDocument = 0 ; SentenceIndexInDocument < DocumentLengthBySentence ; ++SentenceIndexInDocument )
        {
            for (int FeaturesOut_PositionOfSentenceInDocument = 0 ; FeaturesOut_PositionOfSentenceInDocument < FeaturesOut_Embedding_PositionOfSentenceInDocument ; ++FeaturesOut_PositionOfSentenceInDocument )
            {
                Embedding_PositionOfSentenceInDocument [ SentenceIndexInDocument ] [ FeaturesOut_PositionOfSentenceInDocument ] -=

                        Embedding_PositionOfSentenceInDocument_Delta [ SentenceIndexInDocument ] [ FeaturesOut_PositionOfSentenceInDocument ];
            }
        }





        // Not Modify Embedding_Bias Depends On Errors_Embedding_Document
    }





    void Adjustment_00_NormalizationLayer_00_Matrix_Words_and_Features(const int &BlockIndex, const int &EncoderIndex, const int &Rows, const int &Colums)
    {
        Exp = 0;

        Std= 0 ;

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < Rows ; ++WordIndexInDocument )
        {
            for( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
            {
                Exp += Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ];
            }
        }

        Exp = Exp / ( Rows * Colums );

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < Rows ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
            {
                Std +=
                        (
                                (
                                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    -
                                        Exp
                                )
                            *
                                (
                                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    -
                                        Exp
                                )
                        );
            }
        }

        Std = Std / ( Rows * Colums ) + Weights_EpsScalar;

        Std = sqrt (Std );

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < Rows ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
            {
                Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =
                        (
                                (
                                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    -
                                        Exp
                                )
                            /
                                Std
                            *
                                Weights_Gamma [ FeaturesOut ]
                        )
                    +
                        Weights_Beta [ FeaturesOut ];
            }
        }
    }

    void Adjustment_00_NormalizationLayer_01_Vector_Neurons(const int &Layer)
    {
        Exp = 0;

        Std = 0;

        for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
        {
            Exp += Values_FullyConnected [ Layer ] [ Values_Neuron ];
        }

        Exp = Exp / Neurons [ Layer ];

        for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
        {
            Std +=
                (
                        (
                                Values_FullyConnected [ Layer ] [ Values_Neuron ]
                            -
                                Exp
                        )
                    *
                        (
                                Values_FullyConnected [ Layer ] [ Values_Neuron ]
                            -
                                Exp
                        )
                );
        }

        Std = Std / Neurons[Layer] + Weights_EpsScalar_FC;

        Std = sqrt(Std);

        for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
        {
            Values_FullyConnected [ Layer ] [ Values_Neuron ] =
                    (
                            (
                                    Values_FullyConnected [ Layer ] [ Values_Neuron ]
                                -
                                    Exp
                            )
                        /
                            Std
                        *
                            Weights_Gamma_FC
                    )
                +
                    Weights_Beta_FC;
        }
    }

    void Adjustment_01_SpreadingLayer_00_Matrix_Words_and_Features(const int &BlockIndex, const int &EncoderIndex, const int &Rows, const int &Colums)
    {
        // Spreading -->     ( X * Expected Value ) / Standard Division
        // Normalization --> ( X - Expected Value ) / Standard Division

        // Spreading --> when the numbers are ( too far ) try to make them ( more close ) , when the numbers are ( too closer ) try to make them ( more far )
        // Spreading save same signals of numbers

        // Spreading can replace Normalization , I think Spreading are better than Normalization



        Exp = 0;

        Std= 0 ;

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < Rows ; ++WordIndexInDocument )
        {
            for( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
            {
                Exp += Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ];
            }
        }

        Exp = Exp / ( Rows * Colums );

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < Rows ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
            {
                Std +=
                        (
                                (
                                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    -
                                        Exp
                                )
                            *
                                (
                                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    -
                                        Exp
                                )
                        );
            }
        }

        Std = Std / ( Rows * Colums ) + Weights_EpsScalar;

        Std = sqrt (Std );

        for ( int WordIndexInDocument = 0 ; WordIndexInDocument < Rows ; ++WordIndexInDocument )
        {
            for ( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
            {
                Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ] =
                        (
                                (
                                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndexInDocument ] [ FeaturesOut ]
                                    *
                                        Exp
                                )
                            /
                                Std
                            *
                                Weights_Gamma [ FeaturesOut ]
                        )
                    +
                        Weights_Beta [ FeaturesOut ];
            }
        }
    }

    void Adjustment_01_SpreadingLayer_01_Vector_Neurons(const int &Layer)
    {
        // Spreading -->     ( X * Expected Value ) / Standard Division
        // Normalization --> ( X - Expected Value ) / Standard Division

        // Spreading --> when the numbers are ( too far ) try to make them ( more close ) , when the numbers are ( too closer ) try to make them ( more far )
        // Spreading save same signals of numbers

        // Spreading can replace Normalization , I think Spreading are better than Normalization



        Exp = 0;

        Std = 0;

        for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
        {
            Exp += Values_FullyConnected [ Layer ] [ Values_Neuron ];
        }

        Exp = Exp / Neurons [ Layer ];

        for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
        {
            Std +=
                    (
                            (
                                    Values_FullyConnected [ Layer ] [ Values_Neuron ]
                                -
                                    Exp
                            )
                        *
                            (
                                    Values_FullyConnected [ Layer ] [ Values_Neuron ]
                                -
                                    Exp
                            )
                    );
        }

        Std = Std / Neurons[Layer] + Weights_EpsScalar_FC;

        Std = sqrt(Std);

        for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
        {
            Values_FullyConnected [ Layer ] [ Values_Neuron ] =
                    (
                            (
                                    Values_FullyConnected [ Layer ] [ Values_Neuron ]
                                *
                                    Exp
                            )
                        /
                            Std
                        *
                            Weights_Gamma_FC
                    )
                +
                    Weights_Beta_FC;
        }
    }

    void Adjustment_02_ActivationLayer_00_Activation_00_Matrix_Words_and_Features(const int &BlockIndex, const int &EncoderIndex, const int &Rows, const int &Colums, const int &ActivationFunctionTypes)
    {
        if( ActivationFunctionTypes == Linear )
        {
            for ( int WordIndex = 0 ; WordIndex < Rows ; ++WordIndex )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
                {
                    Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] =

                            Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ];
                }
            }
        }



        else if (ActivationFunctionTypes == LeakyReLU )
        {
            for ( int WordIndex = 0 ; WordIndex < Rows ; ++WordIndex )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
                {
                    if ( Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] <= 0 )
                    {
                        Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] =

                                0.01
                            *
                                Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] + Weights_EpsScalar;
                    }
                }
            }
        }



        else if (ActivationFunctionTypes == Tanh )
        {
            for ( int WordIndex = 0 ; WordIndex < Rows ; ++WordIndex )
            {
                for ( int FeaturesOut = 0 ; FeaturesOut < Colums ; ++FeaturesOut )
                {
                    Values_Encoder[ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] =
                            (
                                    exp(Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] )
                                -
                                    exp(-Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] )
                            )
                        /
                            (
                                    exp(Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] )
                                +
                                    exp(- Values_Encoder [ BlockIndex ] [ EncoderIndex ] [ WordIndex ] [ FeaturesOut ] )
                            );
                }
            }
        }
    }

    void Adjustment_02_ActivationLayer_00_Activation_01_Vector_Neurons(const int &Layer, const int &ActivationFunctionTypes)
    {
        if (ActivationFunctionTypes == Linear )
        {
            for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
            {
                Values_FullyConnected [ Layer ] [ Values_Neuron ] =

                    Values_FullyConnected [ Layer ] [ Values_Neuron ];
            }
        }



        else if (ActivationFunctionTypes == LeakyReLU )
        {
            for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
            {
                if ( Values_FullyConnected [ Layer ] [ Values_Neuron ] <= 0 )
                {
                    Values_FullyConnected [ Layer ] [ Values_Neuron ] =

                            0.01
                        *
                            Values_FullyConnected [ Layer ] [ Values_Neuron ];
                }
            }
        }



        else if (ActivationFunctionTypes == Tanh )
        {
            for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
            {
                Values_FullyConnected [ Layer ] [ Values_Neuron ] =
                        (
                                exp(Values_FullyConnected [ Layer ] [ Values_Neuron ] )
                            -
                                exp(- Values_FullyConnected [ Layer ] [ Values_Neuron ] )
                        )
                    /
                        (
                                exp(Values_FullyConnected [ Layer ] [ Values_Neuron ] )
                            +
                                exp(- Values_FullyConnected [ Layer ] [ Values_Neuron ] )
                        );
            }
        }



        else if (ActivationFunctionTypes == Softmax )
        {
            Softmax_Values_FullyConnected = 0;

            for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
            {
                Softmax_Values_FullyConnected +=

                        Values_FullyConnected [ Layer ] [ Values_Neuron ];
            }

            for ( int Values_Neuron = 0 ; Values_Neuron < Neurons [ Layer ] ; ++Values_Neuron )
            {
                Values_FullyConnected [ Layer ] [ Values_Neuron ] =

                        Values_FullyConnected [ Layer ] [ Values_Neuron ]
                    /
                        Softmax_Values_FullyConnected;
            }
        }
    }

    double Adjustment_02_ActivationLayer_01_Derivation_00(const double &Value, const int &ActivationFunctionTypes)
    {
        if ( ActivationFunctionTypes == Linear )
        {
            return 1.00;
        }



        else if ( ActivationFunctionTypes == LeakyReLU )
        {
            if ( Value > 0 ) return 1.00 ;

            else             return 0.01 ;
        }



        else if ( ActivationFunctionTypes == Tanh )
        {
            return 1 - ( Value * Value );
        }



        else if ( ActivationFunctionTypes == Softmax )
        {
            // This Is A Cross Entropy Loos

            return 1;
        }



        return 0;
    }

    void Adjustment_03_SchedulingRates()
    {
        // Scheduling ( MomentumRate && LearningRate ) Of TransformerModel
        for ( int BlockIndex = 0 ; BlockIndex < BlocksOfEncoders ; ++BlockIndex )
        {
            for(int EncoderIndex = 0; EncoderIndex < EncodersNumber_Block [ BlockIndex ]; ++EncoderIndex)
            {
                MomentumRate_TransformerModel [ BlockIndex ] [ EncoderIndex ] *=

                        MomentumRate_TransformerModel_Scheduling [ BlockIndex ] [ EncoderIndex ];



                LearningRate_TransformerModel [ BlockIndex ] [ EncoderIndex ] *=

                        LearningRate_TransformerModel_Scheduling [ BlockIndex ] [ EncoderIndex ];
            }
        }



        // Scheduling ( MomentumRate && LearningRate ) Of EmbeddingLayer
        MomentumRate_EmbeddingLayer *= MomentumRate_EmbeddingLayer_Scheduling;

        LearningRate_EmbeddingLayer *= LearningRate_EmbeddingLayer_Scheduling;



        // Scheduling ( MomentumRate && LearningRate ) Of ClassificationLayer
        MomentumRate_ClassificationLayer *= MomentumRate_ClassificationLayer_Scheduling;

        LearningRate_ClassificationLayer *= LearningRate_ClassificationLayer_Scheduling;
    }





    void ThreadsAssigning_FirstIndexOfMyResponsibilities_and_LastIndexOfMyResponsibilities(const int &ProcessesNumber)
    {
        if (ProcessesNumber > ThreadsAvailable )
        {
            ThreadResponsibilities = ProcessesNumber / ThreadsAvailable;

            for (int ThreadIndex = 0 ; ThreadIndex < ThreadsAvailable ; ++ThreadIndex )
            {
                FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] = ThreadResponsibilities * ThreadIndex;

                LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] = FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] + ThreadResponsibilities - 1;
            }



            // May be ( ProcessesNumber % ThreadsAvailable != 0 ) Must Process The Carry Of NumberOfProcess By Last ThreadIndex
            LastIndexOfMyResponsibilities_Thread [ThreadsAvailable - 1 ] = ProcessesNumber - 1;
        }

        else
        {
            ThreadResponsibilities = 1;

            for (int ThreadIndex = 0 ; ThreadIndex < ProcessesNumber ; ++ThreadIndex )
            {
                FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] = ThreadResponsibilities * ThreadIndex;

                LastIndexOfMyResponsibilities_Thread [ ThreadIndex ] = FirstIndexOfMyResponsibilities_Thread [ ThreadIndex ] + ThreadResponsibilities - 1;
            }
        }



        this->ProcessesNumber = ProcessesNumber;
    }





    void Testing()
    {

    }





    void Save_Embedding_and_Weights()
    {

    }

private:
    // Data Variables ///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int NumberOfTrainingFile;
    unsigned int NumberOFTestingData;
    double Correct;
    PreparedData Data;





    // Initializing Weights Factors /////////////////////////////////////////////////////////////////////////////////////
    double Factor_Embedding_Word;
    double Factor_Embedding_PositionOfWordInSentence;
    double Factor_Embedding_PositionOfSentenceInDocument;
    double Factor_Embedding_Sequence;
    double Factor_Weights_LinearLayer_Block [ BlocksOfEncoders ];
    double Factor_Weights_SumDifferentVisionsOfValues_Block[ BlocksOfEncoders ];
    double Factor_Weights_FullyConnected [ FullyConnectedLayers ];





    // Document Variables ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int IndicesWordsInDictionary_Document [ DocumentLengthByWord ];
    unsigned int IndicesWordsInSentence_Document [ DocumentLengthByWord ];
    unsigned int IndicesSentenceInDocument_Document [ DocumentLengthByWord ];

    double Values_Embedding_Document [ DocumentLengthByWord ] [ FeaturesOut_Embedding_Document ];
    double Values_Embedding_Document_AfterMasking [ DocumentLengthByWord ] [ FeaturesOut_Embedding_Document ];

    double Errors_Embedding_Document [ DocumentLengthByWord ] [ FeaturesOut_Embedding_Document ];

    unsigned int MaskingNumberInSentence [ DocumentLengthBySentence ];
    bool Masks_Document [ DocumentLengthByWord ];

    unsigned int NumberOfWordsInCurrentDocument, LengthOfLargestSentenceInCurrentDocument;

    unsigned int FirstSentenceIndexInDocument, LastSentenceIndexInDocument;

    unsigned int FirstIndexOfThisFeaturesOutIn_Embedding_Document [ 4 ];





    // Embedding Variables //////////////////////////////////////////////////////////////////////////////////////////////
    double Embedding_Word [ maxWords ] [ FeaturesOut_Embedding_Word ];                                                                        // Learnable Variables
    double Embedding_Word_Delta [ maxWords ] [ FeaturesOut_Embedding_Word ];

    // [ Sentence + 1 ] For Special Tokens That Added Before Each Sentence
    double Embedding_PositionOfWordInSentence [SentenceLength + 1 ] [ FeaturesOut_Embedding_PositionOfWordInSentence ];                       // Learnable Variables
    double Embedding_PositionOfWordInSentence_Delta [SentenceLength + 1 ] [ FeaturesOut_Embedding_PositionOfWordInSentence ];

    double Embedding_PositionOfSentenceInDocument [ DocumentLengthBySentence ] [ FeaturesOut_Embedding_PositionOfSentenceInDocument ];        // Learnable Variables
    double Embedding_PositionOfSentenceInDocument_Delta [ DocumentLengthBySentence ] [ FeaturesOut_Embedding_PositionOfSentenceInDocument ];

    double Embedding_Bias [ FeaturesOut_Embedding_Bias ];





    // TransformerModel Variables ///////////////////////////////////////////////////////////////////////////////////////
    double *****Weights_LinearLayer,               // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]    [ EncoderLinearLayer ]    [ FeaturesOut_Block [ i ] ]    [ FeaturesOut_Block [ i ] ];      // Unique
           *****Weights_LinearLayer_Delta,         // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]    [ EncoderLinearLayer ]    [ FeaturesOut_Block [ i ] ]    [ FeaturesOut_Block [ i ] ];      // Unique
            ****Weights_LinearLayer_Sum;           // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]    [ EncoderLinearLayer ]                                   [ FeaturesOut_Block [ i ] ];      // Unique

    double ****Values_InternalAttention,           // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];
           ****Errors_InternalAttention;           // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];

    double ****Values_Quires,                      // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];
           ****Errors_Quires;                      // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];

    double ****Values_Keys,                        // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];
           ****Errors_Keys;                        // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];

    double ****Values_ExternalAttention0,          // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];
           ****Errors_ExternalAttention0;          // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];

    double ****Values_Edges,                       // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ DocumentLengthByWord ];    // Unique
           ****Errors_Edges;                       // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ DocumentLengthByWord ];    // Unique

    double ****Values_ExternalAttention1,          // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];
           ****Errors_ExternalAttention1;          // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];

    double *****Weights_SumDifferentVisions,       // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]    [ 3 ]                     [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ]; // Unique
           *****Weights_SumDifferentVisions_Delta; // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]    [ 3 ]                     [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ]; // Unique

    double ****Values_SumDifferentVisions,         // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];
           ****Errors_SumDifferentVisions;         // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];

    double ****Values_Encoder,                     // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];
           ****Errors_Encoder;                     // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]                              [ DocumentLengthByWord ]             [ FeaturesOut_Block [ i ] ];





    // ClassificationLayer Variables ////////////////////////////////////////////////////////////////////////////////////
    double ***Weights_FullyConnected,              // [ FullyConnectedLayers ]    [ Neurons ]    [ Neurons In Previous Layer ]
           ***Weights_FullyConnected_Delta,        // [ FullyConnectedLayers ]    [ Neurons ]    [ Neurons In Previous Layer ]
            **Weights_FullyConnected_Sum;          // [ FullyConnectedLayers ]    [ Neurons ]

    double **Values_FullyConnected,                // [ FullyConnectedLayers ]    [ Neurons ]
           **Errors_FullyConnected;                // [ FullyConnectedLayers ]    [ Neurons ]





    // Normalization Variables //////////////////////////////////////////////////////////////////////////////////////////
    double Exp, Std;

    double Weights_Gamma [ FeaturesOut_Embedding_Document ],        // Learnable Variable
           Weights_Gamma_Delta [ FeaturesOut_Embedding_Document ];

    double Weights_Beta [ FeaturesOut_Embedding_Document ],         // Learnable Variable
           Weights_Beta_Delta [ FeaturesOut_Embedding_Document ];

    double Weights_EpsScalar,                                       // Learnable Variable
           Weights_EpsScalar_Delta;

    double Weights_Gamma_FC,                                        // Learnable Variable
           Weights_Gamma_FC_Delta;

    double Weights_Beta_FC,                                         // Learnable Variable
           Weights_Beta_FC_Delta;

    double Weights_EpsScalar_FC,                                    // Learnable Variable
           Weights_EpsScalar_FC_Delta;





    // Multi Threading Variables ////////////////////////////////////////////////////////////////////////////////////////
    thread *ThreadPointer;             // [ Available Threads by This Hardware ]
    vector<thread*> ThreadRunning;
    unsigned int ThreadsAvailable, ProcessesNumber, ThreadResponsibilities;
    unsigned int *FirstIndexOfMyResponsibilities_Thread, *LastIndexOfMyResponsibilities_Thread;    // [ Available Threads by This Hardware ]





    // MomentumRate && LearningRate /////////////////////////////////////////////////////////////////////////////////////
    double MomentumRate_ClassificationLayer, LearningRate_ClassificationLayer;
    double MomentumRate_ClassificationLayer_Scheduling, LearningRate_ClassificationLayer_Scheduling;

    double **MomentumRate_TransformerModel, **LearningRate_TransformerModel;                          // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]
    double **MomentumRate_TransformerModel_Scheduling, **LearningRate_TransformerModel_Scheduling;    // [ BlocksOfEncoders ]    [ EncodersNumber_Block [ i ] ]

    double MomentumRate_EmbeddingLayer, LearningRate_EmbeddingLayer;
    double MomentumRate_EmbeddingLayer_Scheduling, LearningRate_EmbeddingLayer_Scheduling;





    // Special HyperParameters //////////////////////////////////////////////////////////////////////////////////////////
    bool AllowOfFlowingFeaturesOutBetweenBlocksDirectly;





    // Temporary Variables //////////////////////////////////////////////////////////////////////////////////////////////
    double Softmax_Values_Edges [ DocumentLengthByWord ]; // Used For Apply Softmax On Values_Edges
    double Softmax_Values_FullyConnected;    // Used For Apply Softmax On Values_FullyConnected
    int Rand0, Rand1, Tmp0, Tmp1;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// classes (end)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// main function
int main()
{
    int Epochs;

    printf("Number Of Epochs:  ");

    scanf("%i",&Epochs); // Epochs == 20

    TransformerModel Model;

    Model.PrepareFunction_00();

    for ( int Epoch = 1 ; Epoch <= Epochs ; ++Epoch ) Model.Training(min ( max ( 10 , Epoch ) , 25 ) , min ( max ( 10 , Epoch ) , 25 ) );

    Model.Testing();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// main function (end)
