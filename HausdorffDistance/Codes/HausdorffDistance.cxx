// Copyright (c) 2011 LTSI INSERM U642
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//     * Neither name of LTSI, INSERM nor the names
// of any contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "vtkHausdorffDistancePointSetFilter.h"

#include "vtkXMLPolyDataReader.h"
#include "vtkPolyDataReader.h"
#include "vtkAlgorithm.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmartPointer.h"
#include "vtkPointSet.h"
#include "vtkFieldData.h"
int main(int argc,char** argv)
{
  int inputAParam = 0;
  int inputBParam = 0;
  int outputAParam = 0;
  int outputBParam = 0;
  int targetParam = 0;
  int printOnlyParam = 0;
  int xmlReader = 0;
  
  for(int i=1;i<argc;i++)
  {
    if(strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"--inputA") == 0)
    {
      inputAParam = i + 1;
    }
    else if(strcmp(argv[i],"-b") == 0 || strcmp(argv[i],"--inputB") == 0)
    {
      inputBParam = i + 1;
    }
    else if(strcmp(argv[i],"-A") == 0 || strcmp(argv[i],"--outputA") == 0)
    {
      outputAParam = i + 1;
    }
    else if(strcmp(argv[i],"-B") == 0 || strcmp(argv[i],"--outputB") == 0)
    {
      outputBParam = i + 1;
    }
    else if(strcmp(argv[i],"-t") == 0 || strcmp(argv[i],"--target") == 0)
    {
      targetParam = i + 1;
    }
    else if(strcmp(argv[i],"-p") == 0 || strcmp(argv[i],"--print-only") == 0)
    {
      printOnlyParam = 1;
    }
    else if(strcmp(argv[i],"-x") == 0 || strcmp(argv[i],"--xml-reader") == 0)
    {
      xmlReader = 1;
    }
  }
  
  if(argc<5)
  {
    cout<<"Missing Parameters!"<<endl;
    cout<<"\nUsage:"<<argv[0]<<endl;
    cout<<"\n(-a , --inputA) InputAFileName"<<endl;
    cout<<"\n(-b , --inputB) InputBFileName"<<endl;
    cout<<"\n(-A , --outputA) OutputAFileName"<<endl;
    cout<<"\n(-B , --outputB) OutputBFileName"<<endl;
    cout<<"\n(-t , --target) <0 point-to-point/1 point-to-cell> (Optional)"<<endl;
    cout<<"\n(-p , --print-only) print on stdout the distances (Optional) Hausdorff AtoB BtoA (space-separated)"<<endl;
    cout<<"\n(-x , --xml-reader) use vtkXMLPolyDataReader as input reader"<<endl;
    return EXIT_FAILURE;
  }
  

  vtkAlgorithm* readerA;  
  vtkAlgorithm* readerB;  
   
  if( xmlReader )
  {
    vtkXMLPolyDataReader* xmlReaderA = vtkXMLPolyDataReader::New();
    readerA = xmlReaderA;

    vtkXMLPolyDataReader* xmlReaderB = vtkXMLPolyDataReader::New();
    readerB = xmlReaderB;

    xmlReaderA->SetFileName(argv[inputAParam]);
    xmlReaderB->SetFileName(argv[inputBParam]);
  }
  else
  {
    vtkPolyDataReader* legacyReaderA = vtkPolyDataReader::New();
    readerA = legacyReaderA;

    vtkPolyDataReader* legacyReaderB = vtkPolyDataReader::New();
    readerB = legacyReaderB;

    legacyReaderA->SetFileName(argv[inputAParam]);
    legacyReaderB->SetFileName(argv[inputBParam]);
  }



 
  vtkSmartPointer<vtkHausdorffDistancePointSetFilter> filter = vtkSmartPointer<vtkHausdorffDistancePointSetFilter>::New();
  filter->SetInputConnection(readerA->GetOutputPort());
  filter->SetInputConnection(1,readerB->GetOutputPort());

  if( atoi(argv[targetParam]) )
    filter->SetTargetDistanceMethod( 1 );
  filter->Update();

  if( !printOnlyParam )
  {
    vtkSmartPointer<vtkPolyDataWriter> polyDataWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
    polyDataWriter->SetInputConnection(filter->GetOutputPort(0));
    polyDataWriter->SetFileName(argv[outputAParam]);
    polyDataWriter->Update();
  
    polyDataWriter->SetInputConnection(filter->GetOutputPort(1));
    polyDataWriter->SetFileName(argv[outputBParam]);
    polyDataWriter->Update();
  }
  else
  {
    std::cout << static_cast<vtkPointSet*>(filter->GetOutput(0))->GetFieldData()->GetArray("HausdorffDistance")->GetComponent(0,0) << " "
              << filter->GetOutputDataObject(0)->GetFieldData()->GetArray("RelativeDistanceAtoB")->GetComponent(0,0) << " "
              << filter->GetOutputDataObject(1)->GetFieldData()->GetArray("RelativeDistanceBtoA")->GetComponent(0,0) << " "
              << std::endl;
  }

  readerA->Delete( );
  readerB->Delete( );

return( EXIT_SUCCESS );  
}
