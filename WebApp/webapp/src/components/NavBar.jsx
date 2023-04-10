import { useNavigate } from "react-router-dom";

const NavBar = () => {

    const nav = useNavigate();
    return (
        <nav className="navbar navbar-expand-lg navbar-light bg-light">
            <div className="collapse navbar-collapse" id="navbarNavAltMarkUp">
                <a classname="nav-link active textColor" href={"/"}>Home</a>
                <a classname="nav-link active textColor" href={"/WOL"}>WOL</a>
            </div>
        </nav>
    );
}

export default NavBar;