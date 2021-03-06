/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2015 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::RASModels::GReWAmm

Group
    grpRASTurbulence

Description
	Wray-Agarwal Turbulence Model

SourceFiles
    GReWAmm.C

\*---------------------------------------------------------------------------*/

#ifndef GReWAmm_H
#define GReWAmm_H

#include "RASModel.H"
#include "eddyViscosity.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace RASModels
{

/*---------------------------------------------------------------------------*\
                      Class GReWAmm Declaration
\*---------------------------------------------------------------------------*/

template<class BasicTurbulenceModel>
class GReWAmm
:
    public eddyViscosity<RASModel<BasicTurbulenceModel> >
{
    // Private Member Functions

        // Disallow default bitwise copy construct and assignment
        GReWAmm(const GReWAmm&);
        GReWAmm& operator=(const GReWAmm&);

protected:

    // Protected data

        // gamma-ReThetat-model coefficients
    
            dimensionedScalar Flength_;
            dimensionedScalar ca2_;
            dimensionedScalar ce2_;
            dimensionedScalar sigmaf_;
            dimensionedScalar TuLim_;
            dimensionedScalar gLim1_;
            dimensionedScalar gLim2_;
            dimensionedScalar RtLim_;
            dimensionedScalar CP3_;
            
        // Model coefficients

            dimensionedScalar Clim_;
            dimensionedScalar kappa_;
            dimensionedScalar Cw_;
            dimensionedScalar C1ke_;
			      dimensionedScalar C1kw_;
            dimensionedScalar sigmake_;
            dimensionedScalar sigmakw_;
            dimensionedScalar Cr0_;
            //dimensionedScalar Cr1_;
            //dimensionedScalar Cr2_;
            dimensionedScalar RH_;
			      dimensionedScalar C2ke_;
            dimensionedScalar C2kw_;
            dimensionedScalar Cmu_;          

        // Fields
            volScalarField dummy_;
            volScalarField dummy2_;
            volScalarField dummy_fonset1_;
            volScalarField dummy_fonset2_;
            volScalarField dummy_fonset3_;
            volScalarField dummy_fonset_;
            volScalarField dummyTu_;
            volScalarField gamma_;
            volScalarField Rnu_;
			      volScalarField f1_;
            volScalarField S_;
            //volScalarField SWdiff_;
            //volScalarField Cras_;

            //- Wall distance
            //  Note: different to wall distance in parent RASModel
            //  which is for near-wall cells only
            
            const volScalarField& y_;
            const volVectorField& n_;    

        // Model functions of gamma-ReThetat-model
    
        tmp<volScalarField> Rt() const;
        tmp<volScalarField> Rv(const volScalarField& S) const;
        tmp<volScalarField> Rv2(const volScalarField& S) const;
        tmp<volScalarField> Tu(const volScalarField& S) const;
        tmp<volScalarField> lambda() const;
        tmp<volScalarField> FPG(const volScalarField& lambda) const;
        tmp<volScalarField> ReThetac(const volScalarField& FPG, const volScalarField& Tu) const;
        tmp<volScalarField> Fonset1(const volScalarField& Rv, const volScalarField& ReThetac) const;
        tmp<volScalarField> Fonset2(const volScalarField& Fonset1) const;
        tmp<volScalarField> Fonset3(const volScalarField& Rt) const;
        tmp<volScalarField> Fturb(const volScalarField& Rt) const;
        tmp<volScalarField> Fonset(const volScalarField& Fonset2, const volScalarField& Fonset3) const;
        tmp<volScalarField> RnuWall(const volScalarField& S) const;
        
    
    // Protected Member Functions

        tmp<volScalarField> chi() const;

        tmp<volScalarField> fv1(const volScalarField& chi) const;

        // Switch function
        tmp<volScalarField> WDF_R
        (
            const volScalarField& S,
            const volScalarField& W    
        ) const;

        tmp<volScalarField> WDF_omega
        (
            const volScalarField& S 
        ) const;

        tmp<volScalarField> WDF_k
        (
            const volScalarField& omega 
        ) const;
        
        tmp<volScalarField> arg1
        (
            const volScalarField& S,
            const volScalarField& W    
        ) const;

        tmp<volScalarField> calcSwitch
        (
            const volScalarField& S,
            const volScalarField& W    
        ) const;

        tmp<volScalarField> blend
        (
            const volScalarField& f1,
            const dimensionedScalar& psi1,
            const dimensionedScalar& psi2
        ) const;

        tmp<volScalarField> sigma(const volScalarField& f1) const
        {
            return blend(f1, sigmakw_, sigmake_);
        }

        tmp<volScalarField> C1(const volScalarField& f1) const
        {
            return blend(f1, C1kw_, C1ke_);
        }

        void correctNut(const volScalarField& fv1);
        virtual void correctNut();

public:

    typedef typename BasicTurbulenceModel::alphaField alphaField;
    typedef typename BasicTurbulenceModel::rhoField rhoField;
    typedef typename BasicTurbulenceModel::transportModel transportModel;


    //- Runtime type information
    TypeName("GReWAmm");

    // Constructors

        //- Construct from components
        GReWAmm
        (
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName = turbulenceModel::propertiesName,
            const word& type = typeName
        );


    //- Destructor
    virtual ~GReWAmm()
    {}


    // Member Functions

        //- Re-read model coefficients if they have changed
        virtual bool read();

        //- Return the effective diffusivity for Rnu
        tmp<volScalarField> DgammaEff() const;
        
        //- Return the effective diffusivity for Rnu
        tmp<volScalarField> DRnuEff(volScalarField f1) const;

        //- Return the turbulence kinetic energy
        virtual tmp<volScalarField> k() const;

        //- Return the turbulence kinetic energy dissipation rate
        virtual tmp<volScalarField> epsilon() const;

        //- Solve the turbulence equations and correct the turbulence viscosity
        virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "GReWAmm.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
