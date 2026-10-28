/*
 Copyright 2013 Adobe Systems Incorporated
 Distributed under the terms of the Gnu General Public License
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// Authors: Nicholas J. Bryan
#include "ThreadHelpers.h"
#include "XMLSerializable.h"
#include "Main.h"
#include "MainContentComp.h"
#include "ISSLogger.h"

 
SaveThread::SaveThread( XMLSerializable * comp_, const File & file_, const File & logFile_ )
    : ThreadWithProgressWindow ("Saving...", false, false), comp(comp_), file(file_), logFile(logFile_),
      result (Result::fail ("Saving did not run"))
    {

    }

void SaveThread::run()
    {

        const MessageManagerLock mml (Thread::getCurrentThread());
        if (! mml.lockWasGained())
        {
            result = Result::fail ("Saving was interrupted");
            return;
        }

        XmlElement xmldoc("PROJECT");

        File assetFolder( file.getParentDirectory().getFullPathName ()
                         + "/" +  file.getFileNameWithoutExtension() + "-assets");

//        assetFolder.deleteRecursively();

        // Stop before writing anything if the asset folder can't be used
        // (e.g. a read-only location such as the root of the disk).
        Result 	res = assetFolder.createDirectory ();
        if (res.failed() || ! assetFolder.hasWriteAccess())
        {
            result = Result::fail ("Could not create a writable folder for the project data:\n"
                                   + assetFolder.getFullPathName()
                                   + (res.failed() ? "\n" + res.getErrorMessage() : String::empty));
            return;
        }

        xmldoc.setAttribute( "assetfolder", file.getFileNameWithoutExtension() + "-assets" );

        // serialize the document state
        const bool assetsSaved = comp->saveToXML(xmldoc, assetFolder);

        // Write to a file
        bool okay =	assetsSaved && xmldoc.writeToFile (file, "");

        xmldoc.deleteAllChildElements ();

        if(okay)
        {
            MAIN_APP->undoManager.clearUndoHistory ();
            result = Result::ok();
        }
        else if (! assetsSaved)
        {
            result = Result::fail ("Some project data could not be written to:\n"
                                   + assetFolder.getFullPathName());
        }
        else
        {
            result = Result::fail ("Could not write the project file.");
        }
}

LoadThread::LoadThread( XMLSerializable * comp_, const File & file_)
:   ThreadWithProgressWindow ("Loading...", false, false),
comp(comp_),
file(file_)
{}

void LoadThread::run()
{
    
    
    // Stop the current engine
    XmlDocument doc(file);
    
    XmlElement * elem = doc.getDocumentElement();
    
    String assetFolder = elem->getStringAttribute("assetfolder");
    
    assetFolder = file.getParentDirectory().getFullPathName() + "/" + assetFolder;
    
    comp->loadFromXML(*elem, assetFolder);
    
    // Delete the xml object after loading
    deleteAndZero(elem);
    
//    MAIN_APP->logger = new ISSLogger(File::getSpecialLocation( File::tempDirectory ).getFullPathName() + "/log.xml", "LOG");
}


 
    
    CreateNewProjectThread::CreateNewProjectThread( MainContentComp * comp_, const File & file_)
    : ThreadWithProgressWindow ("loading...", false, false), comp(comp_), file(file_)
    {}
    
void CreateNewProjectThread::run()
    {
        const MessageManagerLock mml (Thread::getCurrentThread());
        if (! mml.lockWasGained()) return;
        comp->createNewProject(file, false);
    }
    
 
