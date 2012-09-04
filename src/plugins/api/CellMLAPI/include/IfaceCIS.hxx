/* This file is automatically generated from CIS.idl
 * DO NOT EDIT DIRECTLY OR CHANGES WILL BE LOST!
 */
#ifndef _GUARD_IFACE_CIS
#define _GUARD_IFACE_CIS
#include "cda_compiler_support.h"
#ifdef MODULE_CONTAINS_CIS
#define PUBLIC_CIS_PRE CDA_EXPORT_PRE
#define PUBLIC_CIS_POST CDA_EXPORT_POST
#else
#define PUBLIC_CIS_PRE CDA_IMPORT_PRE
#define PUBLIC_CIS_POST CDA_IMPORT_POST
#endif
#include "Ifacexpcom.hxx"
#include "IfaceDOM_APISPEC.hxx"
#include "IfaceMathML_content_APISPEC.hxx"
#include "IfaceCellML_APISPEC.hxx"
#include "IfaceCUSES.hxx"
#include "IfaceAnnoTools.hxx"
#include "IfaceCeVAS.hxx"
#include "IfaceMaLaES.hxx"
#include "IfaceCCGS.hxx"
namespace iface
{
  namespace cellml_services
  {
    typedef std::vector<double>& DoubleSeq;
    typedef enum _enum_ODEIntegrationStepType
    {
      RUNGE_KUTTA_2_3 = 0,
      RUNGE_KUTTA_4 = 1,
      RUNGE_KUTTA_FEHLBERG_4_5 = 2,
      RUNGE_KUTTA_CASH_KARP_4_5 = 3,
      RUNGE_KUTTA_PRINCE_DORMAND_8_9 = 4,
      RUNGE_KUTTA_IMPLICIT_2 = 5,
      RUNGE_KUTTA_IMPLICIT_2_SOLVE = 6,
      RUNGE_KUTTA_IMPLICIT_4 = 7,
      BULIRSCH_STOER_IMPLICIT_BD = 8,
      GEAR_1 = 9,
      GEAR_2 = 10,
      ADAMS_MOULTON_1_12 = 11,
      BDF_IMPLICIT_1_5_SOLVE = 12
    } ODEIntegrationStepType;
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST IntegrationProgressObserver
     : public virtual iface::XPCOM::IObject
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::IntegrationProgressObserver"; }
      virtual ~IntegrationProgressObserver() {}
      virtual void computedConstants(const std::vector<double>& values) throw(std::exception&) = 0;
      virtual void results(const std::vector<double>& state) throw(std::exception&) = 0;
      virtual void done() throw(std::exception&) = 0;
      virtual void failed(const std::string& errorMessage) throw(std::exception&) = 0;
    };
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST CellMLIntegrationRun
     : public virtual iface::XPCOM::IObject
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::CellMLIntegrationRun"; }
      virtual ~CellMLIntegrationRun() {}
      virtual void setStepSizeControl(double epsAbs, double epsRel, double scalVar, double scalRate, double maxStep) throw(std::exception&) = 0;
      virtual void setTabulationStepControl(double tabulationStepSize, bool strictTabulation) throw(std::exception&) = 0;
      virtual void setResultRange(double startBvar, double stopBvar, double maxPointDensity) throw(std::exception&) = 0;
      virtual void setProgressObserver(iface::cellml_services::IntegrationProgressObserver* ipo) throw(std::exception&) = 0;
      virtual void setOverride(iface::cellml_services::VariableEvaluationType type, uint32_t variableIndex, double newValue) throw(std::exception&) = 0;
      virtual void start() throw(std::exception&) = 0;
      virtual void stop() throw(std::exception&) = 0;
    };
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST ODESolverRun
     : public virtual iface::cellml_services::CellMLIntegrationRun
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::ODESolverRun"; }
      virtual ~ODESolverRun() {}
      virtual iface::cellml_services::ODEIntegrationStepType stepType() throw(std::exception&)  = 0;
      virtual void stepType(iface::cellml_services::ODEIntegrationStepType attr) throw(std::exception&) = 0;
    };
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST DAESolverRun
     : public virtual iface::cellml_services::CellMLIntegrationRun
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::DAESolverRun"; }
      virtual ~DAESolverRun() {}
    };
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST CellMLCompiledModel
     : public virtual iface::XPCOM::IObject
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::CellMLCompiledModel"; }
      virtual ~CellMLCompiledModel() {}
      virtual already_AddRefd<iface::cellml_api::Model>  model() throw(std::exception&)  WARN_IF_RETURN_UNUSED = 0;
      virtual already_AddRefd<iface::cellml_services::CodeInformation>  codeInformation() throw(std::exception&)  WARN_IF_RETURN_UNUSED = 0;
    };
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST ODESolverCompiledModel
     : public virtual iface::cellml_services::CellMLCompiledModel
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::ODESolverCompiledModel"; }
      virtual ~ODESolverCompiledModel() {}
    };
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST DAESolverCompiledModel
     : public virtual iface::cellml_services::CellMLCompiledModel
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::DAESolverCompiledModel"; }
      virtual ~DAESolverCompiledModel() {}
    };
    PUBLIC_CIS_PRE 
    class  PUBLIC_CIS_POST CellMLIntegrationService
     : public virtual iface::XPCOM::IObject
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::CellMLIntegrationService"; }
      virtual ~CellMLIntegrationService() {}
      virtual already_AddRefd<iface::cellml_services::ODESolverCompiledModel>  compileModelODE(iface::cellml_api::Model* aModel) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
      virtual already_AddRefd<iface::cellml_services::DAESolverCompiledModel>  compileModelDAE(iface::cellml_api::Model* aModel) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
      virtual already_AddRefd<iface::cellml_services::ODESolverRun>  createODEIntegrationRun(iface::cellml_services::ODESolverCompiledModel* aModel) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
      virtual already_AddRefd<iface::cellml_services::DAESolverRun>  createDAEIntegrationRun(iface::cellml_services::DAESolverCompiledModel* aModel) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
      virtual std::wstring lastError() throw(std::exception&)  WARN_IF_RETURN_UNUSED = 0;
    };
  };
};
#undef PUBLIC_CIS_PRE
#undef PUBLIC_CIS_POST
#endif // guard
