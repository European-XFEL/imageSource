#include "CameraImageSource.hh"

using namespace std;
USING_KARABO_NAMESPACES;

namespace karabo {


    std::string CameraImageSource::scene() {
        const std::string& instanceId = this->getInstanceId();
        std::ostringstream output;
        output << ""
            "<?xml version=\"1.0\" ?>"
            "<svg:svg height=\"484\" krb:uuid=\"1297a61e-d6c6-4cce-b4fb-5133eec3879c\" krb:version=\"2\" width=\"844\" xmlns:krb=\"http://karabo.eu/scene\" xmlns:svg=\"http://www.w3.org/2000/svg\">"
            "	<svg:rect fill=\"none\" height=\"452\" stroke=\"#000000\" stroke-dasharray=\"\" stroke-dashoffset=\"0.0\" stroke-linecap=\"butt\" stroke-linejoin=\"miter\" stroke-miterlimit=\"4.0\" stroke-opacity=\"1.0\" stroke-style=\"1\" stroke-width=\"1.0\" width=\"814\" x=\"6\" y=\"16\"/>"
            "	<svg:rect height=\"27\" krb:class=\"Label\" krb:font=\"Sans Serif11,-1,5,75,0,0,0,0,0\" krb:foreground=\"#000000\" krb:frameWidth=\"0\" krb:text=\"DeviceID\" width=\"74\" x=\"30\" y=\"31\"/>"
            "	<svg:rect expression=\"x.split('/')[-1]\" height=\"27\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".deviceId\" krb:widget=\"Evaluator\" width=\"500\" x=\"120\" y=\"31\"/>"
            "	<svg:g krb:class=\"FixedLayout\" krb:height=\"365\" krb:width=\"810\" krb:x=\"11\" krb:y=\"76\">"
            "		<svg:g krb:class=\"FixedLayout\" krb:height=\"32\" krb:width=\"366\" krb:x=\"456\" krb:y=\"370\">"
            "			<svg:rect height=\"32\" krb:class=\"EditableApplyLaterComponent\" krb:decimals=\"-1\" krb:keys=\"" << instanceId << ".exposureTime\" krb:widget=\"DoubleLineEdit\" width=\"250\" x=\"563\" y=\"370\"/>"
            "			<svg:rect height=\"32\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".exposureTime\" krb:widget=\"DisplayLabel\" width=\"250\" x=\"563\" y=\"411\"/>"
            "			<svg:rect height=\"27\" krb:class=\"Label\" krb:font=\"Sans Serif11,-1,5,50,0,0,0,0,0\" krb:foreground=\"#000000\" krb:frameWidth=\"0\" krb:text=\"Exposure Time\" width=\"110\" x=\"435\" y=\"371\"/>"
            "		</svg:g>"
            "		<svg:g krb:class=\"BoxLayout\" krb:direction=\"0\" krb:height=\"52\" krb:width=\"330\" krb:x=\"465\" krb:y=\"133\">"
            "			<svg:rect height=\"28\" krb:class=\"Label\" krb:font=\"Sans Serif11,-1,5,50,0,0,0,0,0\" krb:foreground=\"#000000\" krb:frameWidth=\"0\" krb:text=\"\" width=\"10\" x=\"0\" y=\"0\"/>"
            "			<svg:rect height=\"24\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".acquire\" krb:requires_confirmation=\"false\" krb:widget=\"DisplayCommand\" width=\"69\" x=\"0\" y=\"0\"/>"
            "		</svg:g>"
            "		<svg:g krb:class=\"BoxLayout\" krb:direction=\"0\" krb:height=\"51\" krb:width=\"330\" krb:x=\"465\" krb:y=\"185\">"
            "			<svg:rect height=\"28\" krb:class=\"Label\" krb:font=\"Sans Serif11,-1,5,50,0,0,0,0,0\" krb:foreground=\"#000000\" krb:frameWidth=\"0\" krb:text=\"\" width=\"10\" x=\"0\" y=\"0\"/>"
            "			<svg:rect height=\"24\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".trigger\" krb:requires_confirmation=\"false\" krb:widget=\"DisplayCommand\" width=\"67\" x=\"0\" y=\"0\"/>"
            "		</svg:g>"
            "		<svg:g krb:class=\"BoxLayout\" krb:direction=\"0\" krb:height=\"52\" krb:width=\"330\" krb:x=\"465\" krb:y=\"236\">"
            "			<svg:rect height=\"28\" krb:class=\"Label\" krb:font=\"Sans Serif11,-1,5,50,0,0,0,0,0\" krb:foreground=\"#000000\" krb:frameWidth=\"0\" krb:text=\"\" width=\"10\" x=\"0\" y=\"0\"/>"
            "			<svg:rect height=\"24\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".stop\" krb:requires_confirmation=\"false\" krb:widget=\"DisplayCommand\" width=\"47\" x=\"0\" y=\"0\"/>"
            "		</svg:g>"
            "		<svg:g krb:class=\"BoxLayout\" krb:direction=\"0\" krb:height=\"51\" krb:width=\"330\" krb:x=\"465\" krb:y=\"288\">"
            "			<svg:rect height=\"28\" krb:class=\"Label\" krb:font=\"Sans Serif11,-1,5,50,0,0,0,0,0\" krb:foreground=\"#000000\" krb:frameWidth=\"0\" krb:text=\"\" width=\"10\" x=\"0\" y=\"0\"/>"
            "			<svg:rect height=\"24\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".reset\" krb:requires_confirmation=\"false\" krb:widget=\"DisplayCommand\" width=\"55\" x=\"0\" y=\"0\"/>"
            "		</svg:g>"
            "		<svg:rect height=\"30\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".state\" krb:staticText=\"\" krb:widget=\"DisplayStateColor\" width=\"290\" x=\"500\" y=\"90\"/>"
            "		<svg:rect height=\"30\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".state\" krb:widget=\"DisplayLabel\" width=\"290\" x=\"500\" y=\"90\"/>"
            "		<svg:rect height=\"390\" krb:class=\"DisplayComponent\" krb:keys=\"" << instanceId << ".output.schema.data.image\" krb:colormap=\"viridis\" krb:widget=\"WebCamGraph\" width=\"430\" x=\"11\" y=\"76\"/>"
            "	</svg:g>"
            "</svg:svg>"
            "";
        return output.str();
    }
}
