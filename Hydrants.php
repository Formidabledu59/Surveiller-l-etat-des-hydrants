<?php

namespace App\Controllers\Api;

use CodeIgniter\RESTful\ResourceController;
use App\Models\Mdonnee;
use App\Controllers\Api\Email;

class Hydrants extends ResourceController
{
    // DÃ©finition du nom du modÃ¨le Ã  utiliser
    protected $modelName = 'App\Models\Mhydrant';

    // Initialisation du modÃ¨le
    protected $model;

    // DÃ©finition du format de rÃ©ponse par dÃ©faut
    protected $format = 'json';


    // MÃ©thode pour rÃ©cupÃ©rer tous les hydrants
    public function index()

    {
        $result = null;
        $session = \Config\Services::session(); //activer les sessions
        if (isset($_SESSION['login']) == TRUE) {
            //si la requÃªte vient d'un utilisateur authentifiÃ©
            $idCommune = $_SESSION['idCommune'];

            // Appel Ã  la mÃ©thode du modÃ¨le pour rÃ©cupÃ©rer tous les hydrants de la commune
            $result = $this->model->getAllcarte($idCommune);
        }
        return $this->respond($result);
    }


    // MÃ©thode pour afficher les dÃ©tails d'un hydrant
    public function show($prmId = null)
    {
        // Appel Ã  la mÃ©thode du modÃ¨le pour rÃ©cupÃ©rer les dÃ©tails d'un hydrant par son ID
        $result = $this->model->getDetail($prmId);

        // VÃ©rification si l'hydrant a Ã©tÃ© trouvÃ©
        if ($result != null) {

            return $this->respond($result);
        } else {
            return $this->respond($result, 400);
        }
    }






    public function create()
    {
        $mdonneeModel = new Mdonnee();

        $data = $this->request->getPost('dto');
        $data = json_decode($data, true);

        if ($data === null) {
            return $this->respond(['error' => 'Invalid data'], 400);
        }

        $idHydrant = null;
        $etat = null;
        $angle = null;
        $temp = null;
        $actifVeille = 0;
        $install = 0;

        if (isset($data['with'][0]['content']['device']) && isset($data['with'][0]['content']['data'])) {
            $prmDevice = $data['with'][0]['content']['device'];
            $prmDonnee = $data['with'][0]['content']['data'];

            $etatHex = substr($prmDonnee, 0, 1);
            $angleHex = substr($prmDonnee, 1, 3);
            $tempHex = substr($prmDonnee, 4, 4);

            $etat = hexdec($etatHex);
            $angle = hexdec($angleHex) / 10;
            $temp = hexdec($tempHex) / 100;
        } else {
            return $this->respond(['error' => 'Invalid data format'], 400);
        }
        $r = $this->model->getIDbyCodeSigFox($prmDevice);
        $idHydrant = $r[0]['idHydrant'];

        switch ($etat) {
            case 0:
                $etatText = "OUVERT";
                $message = "💦 Alerte: L'hydrant " . $idHydrant . " est actuellement OUVERT. Veuillez vérifier immédiatement. Plus d'infos: ";
                break;
            case 1:
                $etatText = "FERME";
                $message = "✅ L'hydrant " . $idHydrant . " est actuellement FERMÉ. Tout est en ordre. Plus d'infos: ";
                
                break;
            case 2:
                $etatText = "RENVERSE";
                $message = "💥 Alerte: L'hydrant " . $idHydrant . " est RENVERSé. Intervention nécessaire. Plus d'infos: ";
                break;
            case 3:
                $etatText = "VEILLE";
                $actifVeille = 1;
                $message = "💤 L'hydrant " . $idHydrant . " est en mode VEILLE. Surveillance activée. Plus d'infos: ";
                break;
            case 4:
                $etatText = "ACTIF";
                $message = "✅ L'hydrant " . $idHydrant . " est ACTIF. Fonctionnement normal. Plus d'infos: ";
                break;
            default:
                $etatText = "FERME";
                $message = "✅ L'hydrant " . $idHydrant . " est actuellement FERMÉ. Tout est en ordre. Plus d'infos: ";
                break;
        }



        $result = $mdonneeModel->createDonnee($idHydrant, $etatText, $angle, $temp);

        $donneesMisesAJour['actifVeille'] = $actifVeille;
        $donneesMisesAJour['renverse'] = $angle;
        $donneesMisesAJour['installOK'] = $install;

        $this->model->updateHydrant($idHydrant, $donneesMisesAJour);

        if ($result != null) {
            $link = "https://surveillancehydrant.alwaysdata.net/Ctableau/detail/" . $idHydrant;
            $emailMessage = $message . $link;

            $email = \Config\Services::email();
            $email->setTo('louisBodin.hydrant@gmail.com');
            $email->setFrom('projet.hydrant@gmail.com', 'Projet Hydrant');
            $email->setSubject('Alerte Hydrant ' . $idHydrant);
            $email->setMessage($emailMessage);

            if ($email->send()) {
                return $this->respond(['message' => 'Email envoyé avec succès'], 200);
            } else {
                $error = $email->printDebugger(['headers']);
                return $this->respond(['error' => $error], 500);
            }
        } else {
            return $this->respond(['error' => 'Failed to create data'], 400);
        }
    }










    ///////////////////////////////////////////////////////////////////////////////



    // MÃ©thode pour mettre Ã  jour un hydrant existant
    public function update($prmId = null)
    {
        // RÃ©cupÃ©ration des donnÃ©es de la requÃªte PUT/PATCH et dÃ©codage en tableau associatif
        $data = $this->request->getRawInput();
        $data = json_decode($data['dto'], true);

        // Appel Ã  la mÃ©thode du modÃ¨le pour mettre Ã  jour l'hydrant avec les donnÃ©es fournies
        $result = $this->model->updateHydrant($prmId, $data);

        // VÃ©rification si la mise Ã  jour a rÃ©ussi
        if ($result != null) {

            return $this->respond($result, 202);
        } else {

            return $this->respond($data, 400);
        }
    }

    // MÃ©thode pour supprimer un hydrant
    public function delete($prmId = null)
    {
        // Appel Ã  la mÃ©thode du modÃ¨le pour supprimer l'hydrant par son ID
        $result = $this->model->deleteHydrant($prmId);

        // VÃ©rification si la suppression a rÃ©ussi
        if ($result != null) {

            return $this->respond($result, 203);
        } else {

            return $this->respond($result, 400);
        }
    }
}
