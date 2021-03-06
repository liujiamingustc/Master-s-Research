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

\*---------------------------------------------------------------------------*/

#include "kklARSM.H"
#include "bound.H"
#include "wallDist.H"
#include "wallFvPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace RASModels
{

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //
template<class BasicTurbulenceModel>
tmp<volScalarField> kklARSM<BasicTurbulenceModel>::chi() const
{
    return nuTilda_/(this->nu());
}

template<class BasicTurbulenceModel>
tmp<volScalarField> kklARSM<BasicTurbulenceModel>::ka
(
    const volScalarField& S
) const
{
    return nuTilda_*S/a1_;
}

template<class BasicTurbulenceModel>
tmp<volScalarField> kklARSM<BasicTurbulenceModel>::len
(
    const volScalarField& S
) const
{
    return pow(nuTilda_,0.5)*pow(S,-0.5);
}

template<class BasicTurbulenceModel>
tmp<volScalarField> kklARSM<BasicTurbulenceModel>::lvkmin
(
    const volScalarField& len
) const
{
    return len/C11_;
}

template<class BasicTurbulenceModel>
tmp<volScalarField> kklARSM<BasicTurbulenceModel>::ep
(
    const volScalarField& ka
) const
{
    return y_*sqrt(scalar(0.3)*ka)/(scalar(20.0)*(this->nu()));
}

template<class BasicTurbulenceModel>
tmp<volScalarField> kklARSM<BasicTurbulenceModel>::fphi
(
    const volScalarField& ep
) const
{
    return (1+Cd1_*ep)/(1+pow(ep,4.0));
}

///////////////////////////////////////////////////////////////////ARSM

template<class BasicTurbulenceModel>
void kklARSM<BasicTurbulenceModel>::correctNut()
{
    this->nut_ = nuTilda_;
    this->nut_.correctBoundaryConditions();

    BasicTurbulenceModel::correctNut();
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class BasicTurbulenceModel>
kklARSM<BasicTurbulenceModel>::kklARSM
(
    const alphaField& alpha,
    const rhoField& rho,
    const volVectorField& U,
    const surfaceScalarField& alphaRhoPhi,
    const surfaceScalarField& phi,
    const transportModel& transport,
    const word& propertiesName,
    const word& type
)
:
    eddyViscosity<RASModel<BasicTurbulenceModel> >
    (
        type,
        alpha,
        rho,
        U,
        alphaRhoPhi,
        phi,
        transport,
        propertiesName
    ),
    
    kappa_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "kappa",
            this->coeffDict_,
            0.41
        )
    ),

    eta1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "eta1",
            this->coeffDict_,
            1.2         
        )
    ),
    
    eta2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "eta2",
            this->coeffDict_,
            0.97
        )
    ),
    
    eta3_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "eta3",
            this->coeffDict_,
            0.13
        )
    ),
    
    Cphi2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cphi2",
            this->coeffDict_,
            eta3_.value()
        )
    ),
    
    f1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "f1",
            this->coeffDict_,
            6.0
        )
    ),

    C11_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C11",
            this->coeffDict_,
            10.0
        )
    ),
    
    C12_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C12",
            this->coeffDict_,
            1.3
        )
    ),

    Cd1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cd1",
            this->coeffDict_,
            4.7
        )
    ),
    
    C3_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C3",
            this->coeffDict_,
            7.0
        )
    ),

    Cmu_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cmu",
            this->coeffDict_,
            0.09
        )
    ),
    
    a1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "a1",
            this->coeffDict_,
            0.3
        )
    ),
    
    pl_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "pl",
            this->coeffDict_,
            1.0
        )
    ),
    
    pk_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "pk",
            this->coeffDict_,
            1.0
        )
    ),
    
    ph_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "ph",
            this->coeffDict_,
            1.0
        )
    ),
    
    pf_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "pf",
            this->coeffDict_,
            1.0
        )
    ),

//////////////////////////////////////////////////////////////////////////ARSM
    
	C1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C1",
            this->coeffDict_,
            1.8
        )
    ),

    C01_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C01",
            this->coeffDict_,
            3.4
        )
    ),

    C22_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C22",
            this->coeffDict_,
            0.36
        )
    ),

    C33_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C33",
            this->coeffDict_,
            1.25
        )
    ),

    C44_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C44",
            this->coeffDict_,
            0.6
        )
    ),

    Cep1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cep1",
            this->coeffDict_,
            1.44
        )
    ),

    Cep2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cep2",
            this->coeffDict_,
            1.83
        )
    ),

    gamma0Star_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "gamma0Star",
            this->coeffDict_,
            C1_.value()/2.0
        )
    ),

    gamma1Star_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "gamma1Star",
            this->coeffDict_,
            C01_.value()+(Cep2_.value()-Cep1_.value())/(Cep1_.value()-1.0)
        )
    ),


    a11_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "a11",
            this->coeffDict_,
            0.5*(4/3-C22_.value())
        )
    ),


    a22_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "a22",
            this->coeffDict_,
            0.5*(2.0-C44_.value())
        )
    ),


    a33_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "a33",
            this->coeffDict_,
            0.5*(2.0-C33_.value())
        )
    ),

    nuTilda_
    (
        IOobject
        (
            "nuTilda",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_
    ),

    al_
    (
        IOobject
        (
            "al",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimensionedScalar("1.0", dimensionSet(0, 0, 2, 0, 0), 1.0)
    ),
    /*
    S_
    (
        IOobject
        (
            "S",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimentionedTensor(S,s., 1.0)
    ),
*/
    y_(wallDist::New(this->mesh_).y())
    
{
    if (type == typeName)
    {
        this->printCoeffs(type);
    }

}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class BasicTurbulenceModel>
bool kklARSM<BasicTurbulenceModel>::read()
{
    if (eddyViscosity<RASModel<BasicTurbulenceModel> >::read())
    {
        return true;
    }
    else
    {
        return false;
    }
}


template<class BasicTurbulenceModel>
tmp<volScalarField> kklARSM<BasicTurbulenceModel>::DnuTildaEff() const
{
    return tmp<volScalarField>
    (
        new volScalarField("DnuTildaEff", nuTilda_*pk_ + this->nu())
    );
}

template<class BasicTurbulenceModel>
void kklARSM<BasicTurbulenceModel>::correct()
{
    if (!this->turbulence_)
    {
        return;
    }

	//Calculate Strain rate magnitude S
    volScalarField S2(2.0*magSqr(symm(fvc::grad(this->U_))));
    volScalarField S = sqrt(S2);
    bound(S, dimensionedScalar("0", S.dimensions(), SMALL));
    bound(S2, dimensionedScalar("0", S2.dimensions(), SMALL));

	// Calculate vorticity magnitude W
    volScalarField W2(2.0*magSqr(skew(fvc::grad(this->U_))));
    volScalarField W = sqrt(W2);
	bound(W, dimensionedScalar("1e-15", W.dimensions(), SMALL));
	bound(W2, dimensionedScalar("1e-15", W2.dimensions(), SMALL));
    
    // Local references
    const alphaField& alpha = this->alpha_;
    const rhoField& rho = this->rho_;
    const surfaceScalarField& alphaRhoPhi = this->alphaRhoPhi_;
    const volVectorField& U = this->U_;
    const volScalarField ka(this->ka(S));
    const volScalarField len(this->len(S));
    const volScalarField lvkmin(this->lvkmin(len));
    const volScalarField ep(this->ep(ka));
    const volScalarField fphi(this->fphi(ep));
	
       
    volScalarField& nut = this->nut_;

    eddyViscosity<RASModel<BasicTurbulenceModel> >::correct();
    
    volScalarField divU(fvc::div(fvc::absolute(this->phi(), U)));
        
    tmp<volTensorField> tgradU = fvc::grad(U);
    volScalarField GP(this->GName(), nut*(dev(twoSymm(tgradU())) && tgradU()));
    tgradU.clear();
   
    //tau_new s
    //tau = -rho*k(beta1_*T1_+beta2_*T2_+beta4_*T4_+2/3delta)
   
    
    volSymmTensorField SS(symm(fvc::grad(U)));   //Strain Rate Tensor
    volTensorField WW(skew(fvc::grad(U)));    //vorticity Tensor  
    volScalarField tau = -1.0/(a1_*al_*S);
    volSymmTensorField S_star = tau*SS;
	volTensorField W_star = tau*WW;
	volTensorField SS_star = S_star & S_star;
	volTensorField SW_star = S_star & W_star;
	volTensorField WS_star = W_star & S_star;
	volTensorField R2 = -(-W_star & W_star) & inv(SS_star);
	volTensorField et2 = SS_star; 
	
	volTensorField T1 = (S_star)-(1/3)*tr(S_star);
   
	//volScalar TRS = tr(S_star);
	//volTensorField T1 = (S_star)-(1/3)*tr(S_star);
   
   /*
    

	volScalarField et = sqr(S_star);

	volScalarField T1 = S_star-tr(S_star)/3.0;
	volScalarField T2 = sqr(S_star) -tr(sqr(S_star))/3.0;
	volScalarField T4 = S_star*W_star-W_star*S_star;
	volScalarField a44_ = tau/(gamma1Star_-2.0*al_*gamma0Star_*sqr(et)*sqr(tau));
	volScalarField beta1 = 2*al_;
	volScalarField beta2 = 2*a44_*a33_*beta1;
	volScalarField beta4 = -a44_*a22_*beta1;

    */
    
/*	



	volScalarField R = -W_star/S_star;
	volScalarField p = - gamma1Star_/(sqr(et)*gamma0Star_);
	volScalarField q = 1.0/sqr((2*sqr(et)*gamma0Star_))*(sqr(gamma1Star_)-2.0*sqr(et)*gamma0Star_*a11_-2/3*sqr(et*a33_)-2*sqr(R*et*a22_));
	volScalarField r = gamma1Star_*a11_/sqr(2*sqr(et)*gamma0Star_);
*/
/*	
	///calculate al_
	volScalarField al;
	forAll(al,cellI)
{
	if (sqr(et[cellI]) < 1e-6) 
		{
		
			volScalarField al[cellI] = - gamma1Star_*a11_/(sqr(gamma1Star_)+2.0*W_star[cellI]*W_star[cellI]*sqr(a22_));
		}
		else 
			{
				volScalarField a[cellI] = (q[cellI]-sqr(p[cellI]));
				volScalarField b[cellI] = (1/27*(2*pow(p[cellI],3)-9.0*p[cellI]*q[cellI]+27*r[cellI]));
				volScalarField d[cellI] = (sqr(b[cellI])/4+pow(a[cellI],3)/27);


				if (d[cellI] > 0)
					{
						volScalarField t1[cellI] = pow(-b[cellI]/2+sqrt(d[cellI]), 1/3);
						volScalarField t2[cellI] = pow(-b[cellI]/2-sqrt(d[cellI]), 1/3);
						volScalarField al[cellI] = min(-p[cellI]/3+t1[cellI]+t2[cellI], -p[cellI]/3-t1[cellI]/2-t2[cellI]/2);
					}	
						else
							{
								volScalarField theta[cellI] = acos(-b[cellI]/(2.0*sqrt(pow(a[cellI],3)/27)));
								volScalarField t1[cellI] = -p[cellI]/3+2.0*sqrt(-a[cellI]/3)*cos(theta[cellI]/3);
								volScalarField t2[cellI] = -p[cellI]/3+2.0*sqrt(-a[cellI]/3)*cos(2.0*
constant::mathematical::pi/3+theta[cellI]/3);
								volScalarField t3[cellI] = -p[cellI]/3+2.0*2*sqrt(-a[cellI]/3)*cos(4/3*
constant::mathematical::pi+theta/3);
								volScalarField al = min(t1[cellI],t2[cellI],t3[cellI]);
							}				

			}	
	al_[cellI] = al[cellI];

}
   */	
    //limiter on G
//	volScalarField G = min(-ka*(beta1*T1+beta2*T2+beta4*T4+2/3)*divU,-Cmu_*nuTilda_/al_);
   // volScalarField G = min(GP-(2.0/3.0)*divU*ka,scalar(20.0)*nuTilda_*S2);
    
    volScalarField fp = min(max(G/(max(
                                        nuTilda_*S2,
                                                    dimensionedScalar("SMALL", dimensionSet(0, 2, -3, 0, 0), 1e-20))),
                                                                                                                        0.5),1.0);
    volScalarField lvkmax = C12_*kappa_*y_*fp;
       
    volScalarField Ux=U.component(0); 
    volScalarField Uy=U.component(1);
    volScalarField Uz=U.component(2);
    volScalarField U2=sqrt(
                            sqr(fvc::laplacian(Ux))
                           +sqr(fvc::laplacian(Uy))
                           +sqr(fvc::laplacian(Uz))
                          );
                 
    volScalarField lvkl = kappa_*mag(S/U2);
    
    //limiter on lvk
    volScalarField lvk = max(lvkmin,min(lvkmax,lvkl));
    volScalarField Cphi1_= eta1_-eta2_*sqr(len/lvk);
    
    //E1e
    volScalarField Eke = sqr(nuTilda_) * magSqr(fvc::grad(S)) / S2;
    volScalarField Ebb = max(magSqr(fvc::grad(nuTilda_)),
                         dimensionedScalar("EbbMin", dimensionSet(0, 2, -2, 0, 0), 1e-15));
    volScalarField E1e = C3_ * Ebb * tanh(Eke/(C3_*Ebb));
    


    tmp<fvScalarMatrix> nuTildaEqn
    (
        fvm::ddt(alpha, rho, nuTilda_)
      + fvm::div(alphaRhoPhi, nuTilda_)
      - fvm::laplacian(alpha*rho*DnuTildaEff(), nuTilda_)
     ==
        alpha*rho*a1_*(Cphi1_-0.5)*G/S
      + alpha*rho*(0.5*a1_-pow(a1_,-2.5)*Cphi2_)*fvm::Sp(S, nuTilda_)/////
      + alpha*rho*(this->nu())*nuTilda_*(3/4-f1_*fphi)/sqr(y_)
      + alpha*rho*pl_*0.5*fvm::Sp((fvc::grad(nuTilda_) & fvc::grad(S))/S, nuTilda_)
      + alpha*rho*ph_*0.75*magSqr(fvc::grad(nuTilda_))
      + alpha*rho*pl_*2.0*sqr(nuTilda_)/sqr(al_)*magSqr(fvc::grad(al_))
      - alpha*rho*pl_*4.0*fvm::Sp(fvc::grad(al_)/al_ & fvc::grad(S), nuTilda_)
      - alpha*rho*pl_*fvm::Sp(fvc::grad(al_)/al_ & fvc::grad(S)/S, sqr(nuTilda_))
      - alpha*rho*pl_*sqr(nuTilda_)/al_*fvm::laplacian(al_)
      - alpha*rho*pf_*0.25*E1e
    );
    
    nuTildaEqn().relax();
    solve(nuTildaEqn);
    bound(nuTilda_, dimensionedScalar("0", nuTilda_.dimensions(), 0.0));
    nuTilda_.correctBoundaryConditions();

    correctNut();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace Foam

// ************************************************************************* //
