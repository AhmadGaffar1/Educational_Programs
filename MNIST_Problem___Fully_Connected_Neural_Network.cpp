// #pragma warning(disable : 4996) // For Ignore Warning For Using (fopen, fread, fwrite, fclose) // use it only in Visual Studio , comment it for any other IDE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Coded By Line Style
// Accuracy ==    9801 / 10000    ==    98.01 %

#include"cmath"
#include"cstdio"
#include"ctime"

#define NumberOfTrainData 60000
#define NumberOfTestData 10000
#define NumberOfPixels 784 // 28*28
#define NumberOfLayers 3
#define MaximumOfNeurons 1000

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Memory Performance:  static local memory   >   static global memory   >   dynamic local memory   >   dynamic global memory

// Memory Size:         static local memory   <   static global memory   <   dynamic local memory   <   dynamic global memory

// Addition Note:       any process on variables in local main() function --> ( faster than all , smaller than all )

// So i used fastest memory which size of it can fit of this problem --> static local memory ( not fit ) , static global memory ( fit )

// So i used static global memory for store all features of class MNIST_NN as you can see below.......

// Final Advice: don't use this approach unless project scope is small, when large projects use pure OOP --> use dynamic local memory --> put all these features in their class

char Buffer [ 200000000 ];

unsigned int LayerID , SourceLayer_NeuronID , TargetLayer_NeuronID , DataSampleID,

             NumberOFEpochs , Result , Correct;

unsigned int NumberOfNeurons_InLayer [ NumberOfLayers ] = {784 ,200 , 10 },

             TrainData [ NumberOfTrainData ] [ NumberOfPixels ] ,TestData [ NumberOfTestData ] [ NumberOfPixels ],

             TrainResult [ NumberOfTrainData ] , TestResult [ NumberOfTestData ];

double  NeuronSummation [ NumberOfLayers ] [ MaximumOfNeurons ],

        NeuronActivation [ NumberOfLayers ] [ MaximumOfNeurons ],

        NeuronError [ NumberOfLayers ] [ MaximumOfNeurons ],

        NeuronWeight [ NumberOfLayers ] [ MaximumOfNeurons ] [ MaximumOfNeurons ],

        NeuronDeltaWeight [ NumberOfLayers ] [ MaximumOfNeurons ] [ MaximumOfNeurons ],

        SumWeights [ NumberOfLayers ] [ MaximumOfNeurons ] ,

        MomentumRate , SchedulingMomentumRate , LearningRate , SchedulingLearningRate,

        NewDelta , SigmoidConstant , Factor;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MNIST_NN
{
public:
    MNIST_NN()
    {
        NumberOFEpochs = 20;

        MomentumRate = 0.7; SchedulingMomentumRate = 1.0;

        LearningRate = 0.3; SchedulingLearningRate = 0.9;



        NeuronSummation [ 0 ] [ NumberOfNeurons_InLayer [ 0 ] ] = 1.0; // bias Layer[0] = 1

        NeuronSummation [ 1 ] [ NumberOfNeurons_InLayer [ 1 ] ] = 1.0; // bias Layer[1] = 1

        NeuronSummation [ 2 ] [ NumberOfNeurons_InLayer [ 2 ] ] = 1.0; // bias Layer[2] = 1 // (Recycle Bin) Not Used Now


        // Sigmoid Constant may by [   0.1   :   1.0   ]
        // Decreasing Sigmoid Constant For Decreasing Saturating Chance
        SigmoidConstant = 0.2;

        Correct = 0;

        InitialWeight();
    }

    void training()
    {
        // Pass On All Training Data
        for ( DataSampleID = 0 ; DataSampleID < NumberOfTrainData ; ++DataSampleID )
        {
            // ZeroInitialization ( NeuronSummation , NeuronActivation , NeuronError )
            for ( LayerID = 0 ; LayerID < NumberOfLayers ; ++LayerID )
            {
                for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ] ; ++TargetLayer_NeuronID )
                {
                    NeuronSummation [ LayerID ] [ TargetLayer_NeuronID] = 
                        
                    NeuronActivation [ LayerID ] [ TargetLayer_NeuronID] = 
                        
                    NeuronError [ LayerID ] [ TargetLayer_NeuronID] = 0;
                }
            }





            // ForwardPropagation --> Calculate ( Summation Function , Activation Function )
            
            // Layer [ InputLayer ]   ( InputLayer ==  0 )   ( Summation Function , Activation Function )
            for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfPixels ; ++TargetLayer_NeuronID )
            {
                // ( Summation Function )
                // ( minimum value == 0 , Maximum value = 256 )   -->   we divide each value on maximum value ( 256 ) for scaling value between ( 0 : 1 ) 
                NeuronSummation [ 0 ] [ TargetLayer_NeuronID ] = TrainData [ DataSampleID ] [ TargetLayer_NeuronID ] / 256.0 + 0.0001;
                
                // ( Activation Function )
                NeuronActivation [ 0 ] [ TargetLayer_NeuronID ] = NeuronSummation [ 0 ] [ TargetLayer_NeuronID ];
            }



            // Layer [ HiddenLayers , OutputLayer ]   ( HiddenLayers ==  1 : LastLayer - 1 )   ( OutputLayer == LastLayer )   ( Summation Function , Activation Function )
            for ( LayerID = 1 ; LayerID < NumberOfLayers ; ++LayerID )
            {
                // ( Summation Function )
                for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ] ; ++TargetLayer_NeuronID )
                {
                    for ( SourceLayer_NeuronID = 0 ; SourceLayer_NeuronID <= NumberOfNeurons_InLayer [ LayerID - 1 ] ; ++SourceLayer_NeuronID )
                    {
                        NeuronSummation [ LayerID ] [ TargetLayer_NeuronID ] += 

                                NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ]
                            *
                                NeuronActivation [ LayerID - 1 ] [ SourceLayer_NeuronID ];    
                    }
                }



                // ( Activation Function )   in this case we use same Activation Function ( Sigmoid ) for ( HiddenLayers , OutputLayer )

                // if ( LayerID == HiddenLayer )   -->   Apply Sigmoid Equation
                if ( LayerID < NumberOfLayers - 1 )
                {
                    for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ] ; ++TargetLayer_NeuronID )
                    {
                        // NeuronActivation == Sigmoid Equation On ( X == NeuronSummation )
                        NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] = 
                                
                                SigmoidConstant 
                            / 
                                ( SigmoidConstant + exp ( - NeuronSummation [ LayerID ] [ TargetLayer_NeuronID ] ) );
                    }
                }

                // if ( LayerID == OutputLayer )   -->   Apply Sigmoid Equation
                else // if ( LayerID == NumberOfLayers - 1 )
                {
                    for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ] ; ++TargetLayer_NeuronID )
                    {
                        // NeuronActivation == Sigmoid Equation On ( X == NeuronSummation )
                        NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] = 
                                
                                SigmoidConstant 
                            / 
                                ( SigmoidConstant + exp ( - NeuronSummation [ LayerID ] [ TargetLayer_NeuronID ] ) );
                    }
                }
            }





            // BackPropagation --> ( Calculate Errors , Modify Weigths )

            
            
            // Calculate Errors
            
            // Layer [ OutputLayer ]    ( OutputLayer == LastLayer )   Calculate Errors For All Neurons
            for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID <= NumberOfNeurons_InLayer [ NumberOfLayers - 1 ] ; ++TargetLayer_NeuronID)
            {
                // Calculate Error   -->   if ( Truth Result == 0 ) 
                if ( TargetLayer_NeuronID != TrainResult [ DataSampleID ] )
                {
                    NeuronError [ NumberOfLayers - 1 ] [ TargetLayer_NeuronID ]   =   NeuronActivation [ NumberOfLayers - 1 ] [ TargetLayer_NeuronID ] - 0.0;
                }

                // Calculate Error   -->   if ( Truth Result == 1 ) 
                else // if ( TargetLayer_NeuronID == TrainResult [ DataSampleID ] )
                {
                    NeuronError [ NumberOfLayers - 1 ] [ TargetLayer_NeuronID ]   =   NeuronActivation [ NumberOfLayers - 1 ] [ TargetLayer_NeuronID ] - 1.0;
                }
            }


            
            // Layer [ HiddenLayers ]   ( HiddenLayers ==  1 : LastLayer - 1 )   Calculate Errors For All Neurons
            for ( LayerID = NumberOfLayers - 2 ; LayerID > 0 ; --LayerID )
            {
                for ( SourceLayer_NeuronID = 0 ; SourceLayer_NeuronID <= NumberOfNeurons_InLayer [ LayerID ] ; ++SourceLayer_NeuronID )
                {
                    for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID + 1 ] ; ++TargetLayer_NeuronID )
                    {
                            NeuronError [ LayerID ] [ SourceLayer_NeuronID ] += 

                                    NeuronWeight [ LayerID + 1 ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ]   
                                /   
                                    SumWeights [ LayerID + 1 ] [ TargetLayer_NeuronID ]  
                                *   
                                    NeuronError [ LayerID + 1 ] [ TargetLayer_NeuronID ];
                    }
                }
            }



            // Layer [ InputLayer ]   ( InputLayer == 0 )   Calculate Errors For All Neurons
            // InputLauer not have errors, because InputLayer not have Weights , because InputLayer take her values from Data



            // Modify Weigths

            // Modify NeuronWeights For ( OutputLayer , HiddenLayers )   ( InputLayer == 0   Not have weights ) 
            for ( LayerID = NumberOfLayers - 1 ; LayerID > 0 ; --LayerID )
            {
                for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ] ; ++TargetLayer_NeuronID )
                {
                    for ( SourceLayer_NeuronID = 0 ; SourceLayer_NeuronID <= NumberOfNeurons_InLayer [ LayerID - 1 ] ; ++SourceLayer_NeuronID )
                    {

                        // if ( LayerID == HiddenLayer )   -->   Apply Sigmoid Derivative
                        if ( LayerID < NumberOfLayers - 1 )
                        {
                            NewDelta =

                                    // Error Function Derivative
                                    ( 2 * NeuronError [ LayerID ] [ TargetLayer_NeuronID ] )
                                * 
                                    // Activation Function Derivative
                                    NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] * ( 1.0 - NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] ) 
                                *
                                    // Summation Function Derivative
                                    NeuronActivation [ LayerID - 1 ] [ SourceLayer_NeuronID ];
                        }

                        // if ( LayerID == OutputLayer )   -->   Apply Sigmoid Derivative
                        else // if ( LayerID == NumberOfLayers - 1 )
                        {
                            NewDelta =

                                    // Error Function Derivative
                                    ( 2 * NeuronError [ LayerID ] [ TargetLayer_NeuronID ] )
                                *
                                    // Activation Function Derivative
                                    NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] * ( 1.0 - NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] )
                                *
                                    // Summation Function Derivative
                                    NeuronActivation [ LayerID - 1 ] [ SourceLayer_NeuronID ];
                        }



                        NeuronDeltaWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ] = 
                                
                                 NeuronDeltaWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ] * MomentumRate    
                            +   
                                 NewDelta * LearningRate;



                        SumWeights [ LayerID ] [ TargetLayer_NeuronID ] -= abs ( NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ] );

                        NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ] -= NeuronDeltaWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ];

                        SumWeights [ LayerID ] [ TargetLayer_NeuronID ] += abs ( NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ] );
                    }
                }
            }

        }
    }

    void testing()
    {
        for ( DataSampleID = 0 ; DataSampleID < NumberOfTestData ; ++DataSampleID )
        {
            // ZeroInitialization ( NeuronSummation , NeuronActivation , NeuronError )
            for ( LayerID = 0; LayerID < NumberOfLayers; ++LayerID )
            {
                for ( TargetLayer_NeuronID = 0; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ]; ++TargetLayer_NeuronID )
                {
                    NeuronSummation [ LayerID ] [ TargetLayer_NeuronID ] = 
                        
                    NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] = 
                     
                    NeuronError [ LayerID ] [ TargetLayer_NeuronID ] = 0;
                }
            }





            // ForwardPropagation --> Calculate ( Summation Function , Activation Function )

            // Layer [ InputLayer ]   ( InputLayer ==  0 )   ( Summation Function , Activation Function )
            for ( TargetLayer_NeuronID = 0; TargetLayer_NeuronID < NumberOfPixels; ++TargetLayer_NeuronID )
            {
                // ( Summation Function )
                // ( minimum value == 0 , Maximum value = 256 )   -->   we divide each value on maximum value ( 256 ) for scaling value between ( 0 : 1 ) 
                NeuronSummation [ 0 ] [ TargetLayer_NeuronID ] = TestData [ DataSampleID ] [ TargetLayer_NeuronID ] / 256.0 + 0.0001;
                    
                // ( Activation Function )
                NeuronActivation [ 0 ] [ TargetLayer_NeuronID ] = NeuronSummation [ 0 ] [ TargetLayer_NeuronID ];
            }



            // Layer [ HiddenLayers , OutputLayer ]   ( HiddenLayers ==  1 : LastLayer - 1 )   ( OutputLayer == LastLayer )   ( Summation Function , Activation Function )
            for ( LayerID = 1; LayerID < NumberOfLayers; ++LayerID )
            {
                // ( Summation Function )
                for ( TargetLayer_NeuronID = 0; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ]; ++TargetLayer_NeuronID )
                {
                    for ( SourceLayer_NeuronID = 0; SourceLayer_NeuronID <= NumberOfNeurons_InLayer [ LayerID - 1 ]; ++SourceLayer_NeuronID )
                    {
                        NeuronSummation [ LayerID ] [ TargetLayer_NeuronID ] +=

                                NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ]
                            *
                                NeuronActivation [ LayerID - 1 ] [ SourceLayer_NeuronID ];
                    }
                }



                // ( Activation Function )   in this case we use same Activation Function ( Sigmoid ) for ( HiddenLayers , OutputLayer )

                // if ( LayerID == HiddenLayer )   -->   Apply Sigmoid Equation
                if ( LayerID < NumberOfLayers - 1 )
                {
                    for ( TargetLayer_NeuronID = 0; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ]; ++TargetLayer_NeuronID )
                    {
                        // NeuronActivation == Sigmoid Equation On ( X == NeuronSummation )
                        NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] =

                                SigmoidConstant
                            /
                                ( SigmoidConstant + exp( - NeuronSummation [ LayerID ] [ TargetLayer_NeuronID ] ) );
                    }
                }

                // if ( LayerID == OutputLayer )   -->   Apply Sigmoid Equation
                else // if ( LayerID == NumberOfLayers - 1 )
                {
                    for ( TargetLayer_NeuronID = 0; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ]; ++TargetLayer_NeuronID )
                    {
                        // NeuronActivation == Sigmoid Equation On ( X == NeuronSummation )
                        NeuronActivation [ LayerID ] [ TargetLayer_NeuronID ] =

                                SigmoidConstant
                            /
                                ( SigmoidConstant + exp( - NeuronSummation [ LayerID ] [ TargetLayer_NeuronID ] ) );
                    }
                }
            }




            Result = 0;

            for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < 10 ; ++TargetLayer_NeuronID )
            {
                if ( NeuronActivation [ NumberOfLayers - 1 ] [ TargetLayer_NeuronID ] > NeuronActivation [ NumberOfLayers - 1 ] [ Result ] )
                {
                    Result = TargetLayer_NeuronID;
                }
            }

            if ( Result == TestResult [ DataSampleID ] )
            {
                ++Correct;
            }
        }
    }



private:

    void InitialWeight()
    {
        // ZeroInitialization ( SumWeights )
        for ( LayerID = 0 ; LayerID < NumberOfLayers ; ++LayerID )
        {
            for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < MaximumOfNeurons ; ++TargetLayer_NeuronID )
            {
                SumWeights [ LayerID ] [ TargetLayer_NeuronID ] = 0;
            }
        }



        int RandomNumber;

        for ( LayerID = 1 ; LayerID < NumberOfLayers ; ++LayerID )
        {
            // we use ( Factor ) for scaling Weights
            // Factor == Max Random Value Can Generated By rand() Function   *   Number Of Neurons In Previous Layer ^ 0.5
            Factor = ( double ) ( 1 << 15 )   *   pow ( NumberOfNeurons_InLayer [ LayerID - 1 ] , 0.5 );



            // Weights Initialization
            for ( TargetLayer_NeuronID = 0 ; TargetLayer_NeuronID < NumberOfNeurons_InLayer [ LayerID ] ; ++TargetLayer_NeuronID )
            {
                for ( SourceLayer_NeuronID = 0 ; SourceLayer_NeuronID <= NumberOfNeurons_InLayer [ LayerID - 1 ] ; ++SourceLayer_NeuronID )
                {
                    RandomNumber = rand();

                    NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ] = RandomNumber / Factor;

                    SumWeights [ LayerID ] [ TargetLayer_NeuronID ] += NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ];



                    // for make ( 1 / 3 ) is Negative   &&   stay ( 2 / 3 ) is Positive 
                    if ( RandomNumber % 3 == 0 )
                    {
                        NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ] = 
                            
                            - NeuronWeight [ LayerID ] [ TargetLayer_NeuronID ] [ SourceLayer_NeuronID ];
                    }
                }
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    int CharID, Pixel, SampleID;
    FILE* f;

    // Reading Data From File --> ( mnist_train.csv )
    f = fopen ( "mnist_train.csv" , "r" );
    fread ( Buffer , 200000000 , 1 , f );
    fclose ( f );

    for ( CharID = 0, SampleID = 0; SampleID < NumberOfTrainData; ++SampleID )
    {
        TrainResult [ SampleID ] = Buffer [ CharID ] - '0';

        CharID += 2;

        for ( Pixel = 0; Pixel < NumberOfPixels; ++Pixel, ++CharID )
        {
            for ( ; Buffer [ CharID ] >= '0' && Buffer [ CharID ] <= '9'; ++CharID )
            {
                TrainData [ SampleID ] [ Pixel ] = TrainData [ SampleID ] [ Pixel ] * 10 + Buffer [ CharID ] - '0';
            }
        }
    }



    // Clearing Buffer
    for ( SampleID = 0; SampleID < 200000000; ++SampleID ) Buffer [ SampleID ] = '\0';



    // Reading Data From File --> ( mnist_test.csv )
    f = fopen ( "mnist_test.csv" , "r" );
    fread ( Buffer , 200000000 , 1 , f );
    fclose ( f );
    for ( CharID = 0, SampleID = 0; SampleID < NumberOfTestData; ++SampleID )
    {
        TestResult [ SampleID ] = Buffer [ CharID ] - '0';

        CharID += 2;

        for ( Pixel = 0; Pixel < NumberOfPixels; ++Pixel, ++CharID )
        {
            for ( ; Buffer [ CharID ] >= '0' && Buffer [ CharID ] <= '9'; ++CharID )
            {
                TestData [ SampleID ] [ Pixel ] = TestData [ SampleID ] [ Pixel ] * 10 + Buffer [ CharID ] - '0';
            }
        }
    }



    // for calcualte time for ( Training   &&   Testing )
    int start, end;
    start = clock();



    MNIST_NN Agent;

    printf ( "Number Of Epochs == %i\n\n", NumberOFEpochs );
    printf ( "Momentum Rate == %f\n", MomentumRate );
    printf ( "Scheduling Momentum Rate == %f\n\n", SchedulingMomentumRate );
    printf ( "Learning Rate == %f\n", LearningRate );
    printf ( "Scheduling Learning Rate == %f\n\n", SchedulingLearningRate );



    // Training Neural Network for ( Number Of Epochs )
    for ( int Epoch = 1; Epoch <= NumberOFEpochs; ++Epoch )
    {
        printf ( "Epoch :: %i\n", Epoch );

        Agent.training();

        LearningRate *= SchedulingLearningRate;
    }

    // Testing Neural Network
    Agent.testing();



    end = clock();

    printf ( "Correct == %i\n", Correct );
    printf ( "Time == %i\n", ( end - start ) / 1000 );



    return 0;
}
