//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _TAML_ASSET_DECLARED_UPDATE_VISITOR_H_
#define _TAML_ASSET_DECLARED_UPDATE_VISITOR_H_

#ifndef _TAML_XMLPARSER_H_
#include "persistence//taml/tamlXmlParser.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

#ifndef _STRINGTABLE_H_
#include "string/stringTable.h"
#endif

#ifndef _ASSET_FIELD_TYPES_H_
#include "assets/assetFieldTypes.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

class TamlAssetDeclaredUpdateVisitor : public TamlXmlVisitor
{
protected:
    virtual bool visit( TiXmlElement* pXmlElement, TamlXmlParser& xmlParser )
    {
        // Debug Profiling.
        PROFILE_SCOPE(TamlAssetDeclaredUpdateVisitor_VisitElement);

        // Finish if this is not the root element.
        if ( pXmlElement != pXmlElement->GetDocument()->RootElement() )
            return true;

        // Fetch asset field names.
        StringTableEntry assetNameField = StringTable->insert( ASSET_BASE_ASSETNAME_FIELD );

        // Iterate attributes.
        for ( TiXmlAttribute* pAttribute = pXmlElement->FirstAttribute(); pAttribute; pAttribute = pAttribute->Next() )
        {
            // Insert attribute name.
            StringTableEntry attributeName = StringTable->insert( pAttribute->Name() );

            // Asset name?
            if ( attributeName != assetNameField )
                continue;

            // Is this the asset Id we're looking for?
            if ( dStricmp( pAttribute->Value(), mAssetNameFrom ) != 0 )
            {
                // No, so warn.
                Con::warnf("Cannot rename asset Name '%s' to asset Name '%s' as the declared asset Name was %s",
                    mAssetNameFrom, mAssetNameTo, pAttribute->Value() );

                // Stop processing!
                return false;
            }

            // Assign new value.
            pAttribute->SetValue( mAssetNameTo );

            // Stop processing!
            return false;
        }

        return true;
    }

    virtual bool visit( TiXmlAttribute* pAttribute, TamlXmlParser& xmlParser ) { return true; }

public:
    TamlAssetDeclaredUpdateVisitor() {}
    virtual ~TamlAssetDeclaredUpdateVisitor() {}

    bool parse( const char* pFilename )
    {
        TamlXmlParser parser;
        return parser.parse( pFilename, *this, true );
    }

    void setAssetIdFrom( const char* pAssetIdFrom )
    {
        // Sanity!
        AssertFatal( pAssetIdFrom != NULL, "Asset Id from cannot be NULL." );

        // Reset asset Id.
        mAssetIdFrom = StringTable->EmptyString;
        mAssetNameFrom = StringTable->EmptyString;

        // Is asset Id the correct format?
        if ( StringUnit::getUnitCount( pAssetIdFrom, ASSET_SCOPE_TOKEN ) != 2 )
        {
            // No, so warn.
            Con::warnf( "TamlAssetDeclaredUpdateVisitor::setAssetIdFrom() - Cannot use asset Id '%s' as it is not the correct format.", pAssetIdFrom );
            return;
        }

        // Set asset Id.
        mAssetIdFrom = StringTable->insert( pAssetIdFrom );
        mAssetNameFrom = StringTable->insert( StringUnit::getUnit( pAssetIdFrom, 1, ASSET_SCOPE_TOKEN ) );
    }
    StringTableEntry getAssetIdFrom( void ) const { return mAssetIdFrom; }

    void setAssetIdTo( const char* pAssetIdTo )
    {
        // Sanity!
        AssertFatal( pAssetIdTo != NULL, "Asset Id to cannot be NULL." );

        // Reset asset Id.
        mAssetIdTo = StringTable->EmptyString;
        mAssetNameTo = StringTable->EmptyString;

        // Is asset Id the correct format?
        if ( StringUnit::getUnitCount( pAssetIdTo, ASSET_SCOPE_TOKEN ) != 2 )
        {
            // No, so warn.
            Con::warnf( "TamlAssetDeclaredUpdateVisitor::setAssetIdTo() - Cannot use asset Id '%s' as it is not the correct format.", pAssetIdTo );
            return;
        }

        // Set asset Id.
        mAssetIdTo = StringTable->insert( pAssetIdTo );
        mAssetNameTo = StringTable->insert( StringUnit::getUnit( pAssetIdTo, 1, ASSET_SCOPE_TOKEN ) );
    }
    const char* getAssetIdTo( void ) const { return mAssetIdTo; }

private:    
    StringTableEntry mAssetIdFrom;
    StringTableEntry mAssetIdTo;
    StringTableEntry mAssetNameFrom;
    StringTableEntry mAssetNameTo;
};

#endif // _TAML_ASSET_DECLARED_UPDATE_VISITOR_H_
