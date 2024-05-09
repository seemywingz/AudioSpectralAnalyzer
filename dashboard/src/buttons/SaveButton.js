
function SaveButton({ config, saveConfig }) {

    const handleSave = () => {
        saveConfig({ ...config });
    };

    return (
        <button onClick={handleSave} style={{ color: "black", backgroundColor: '#38ffb9' }}>Save Current Config</button>
    );
}
export default SaveButton;
